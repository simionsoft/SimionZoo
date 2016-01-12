#include "stdafx.h"
#include "critic.h"
#include "vfa.h"
#include "parameters.h"
#include "parameter.h"
#include "features.h"
#include "globals.h"
#include "experiment.h"


CTDLambdaCritic::CTDLambdaCritic(CParameters *pParameters) : CVFACritic(pParameters)
{
	m_z= new CFeatureList();
	m_aux= new CFeatureList();

	m_pAlpha= pParameters->addParameter(CParameter("LEARNING_RATE",0.0));
	m_gamma= pParameters->getParameter("INITIAL_GAMMA")->getDouble();
	m_lambda= pParameters->getParameter("INITIAL_LAMBDA")->getDouble();

	
}

CTDLambdaCritic::~CTDLambdaCritic()
{
	delete m_z;
	delete m_aux;
}

double CTDLambdaCritic::updateValue(CState *s, CAction *a, CState *s_p, double r,double rho)
{
	if (m_pAlpha->getDouble()==0.0) return 0.0;
	//using sample importance
	//z= gamma * lambda * rho * z + phi_v(s)
	if (g_pExperiment->m_expProgress.isFirstStep())
		m_z->clear();
	m_z->mult(m_lambda*m_gamma*rho);
	m_z->applyThreshold(0.0001);

	m_pVFA->getFeatures(s,0,m_aux);
	m_z->addFeatureList(m_aux,m_pAlpha->getDouble(),false,true);


	//theta= theta + alpha(r + gamma*newValue - oldValue)*z
	double oldValue= m_pVFA->getValue(m_aux);

	m_pVFA->getFeatures(s_p,0,m_aux);
	double newValue= m_pVFA->getValue(m_aux);
	double td= rho*r + m_gamma*newValue - oldValue;
	
	m_pVFA->add(m_z,td);

	return td;
}

