#include "stdafx.h"
#include "critic.h"
#include "vfa.h"
#include "features.h"
#include "globals.h"
#include "experiment.h"
#include "vfa-critic.h"
#include "etraces.h"
#include "parameters.h"

CTDLambdaCritic::CTDLambdaCritic(CParameters *pParameters)
	: CVFACritic(pParameters)
{
	m_z= new CETraces("Critic/E-Traces",pParameters->getChild("E-Traces"));
	m_aux= new CFeatureList("Critic/aux");
	m_pAlpha = pParameters->getNumericHandler("Alpha");
	m_pGamma = pParameters->getNumericHandler("Gamma");
}

CTDLambdaCritic::~CTDLambdaCritic()
{
	delete m_z;
	delete m_aux;
}

double CTDLambdaCritic::updateValue(const CState *s, const CAction *a, const CState *s_p, double r,double rho)
{
	double alpha = m_pAlpha->getValue();
	if (alpha==0.0) return 0.0;

	//using sample importance
	//z= gamma * lambda * rho * z + phi_v(s)

	double gamma = m_pGamma->getValue();
	m_z->update(gamma*rho);

	m_pVFA->getFeatures(s, m_aux);
	m_z->addFeatureList(m_aux,alpha);


	//theta= theta + alpha(r + gamma*newValue - oldValue)*z
	double oldValue= m_pVFA->getValue(m_aux);

	m_pVFA->getFeatures(s_p, m_aux);
	double newValue= m_pVFA->getValue(m_aux);
	double td = rho*r + gamma*newValue - oldValue;
	
	m_pVFA->add(m_z,td);

	return td;
}

