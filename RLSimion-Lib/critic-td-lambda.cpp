#include "stdafx.h"
#include "critic.h"
#include "vfa.h"
#include "features.h"
#include "globals.h"
#include "experiment.h"
#include "vfa-critic.h"
#include "etraces.h"
#include "xml-parameters.h"

CTDLambdaCritic::CTDLambdaCritic(tinyxml2::XMLElement *pParameters)
	: CVFACritic(pParameters)
{
	m_z= new CETraces("Critic\\E-Traces",pParameters->FirstChildElement("E-Traces"));
	m_aux= new CFeatureList("Critic\\aux");
	m_pAlpha = XMLUtils::getNumericHandler(pParameters->FirstChildElement("Alpha"));
	m_pGamma = XMLUtils::getNumericHandler(pParameters->FirstChildElement("Gamma"));
}

CTDLambdaCritic::~CTDLambdaCritic()
{
	delete m_z;
	delete m_aux;
}

double CTDLambdaCritic::updateValue(CState *s, CAction *a, CState *s_p, double r,double rho)
{
	double alpha = m_pAlpha->getValue();
	if (alpha==0.0) return 0.0;

	//using sample importance
	//z= gamma * lambda * rho * z + phi_v(s)

	double gamma = m_pGamma->getValue();
	m_z->update(gamma*rho);

	m_pVFA->getFeatures(s,0,m_aux);
	m_z->addFeatureList(m_aux,alpha);


	//theta= theta + alpha(r + gamma*newValue - oldValue)*z
	double oldValue= m_pVFA->getValue(m_aux);

	m_pVFA->getFeatures(s_p,0,m_aux);
	double newValue= m_pVFA->getValue(m_aux);
	double td = rho*r + gamma*newValue - oldValue;
	
	m_pVFA->add(m_z,td);

	return td;
}

