#include "stdafx.h"
#include "critic.h"
#include "vfa.h"
#include "parameters.h"
#include "features.h"
#include "globals.h"
#include "experiment.h"

CTDLambdaCritic::CTDLambdaCritic(CParameters *pParameters)
{
	m_pVFA= new CRBFFeatureGridVFA(pParameters->getStringPtr("SIMGOD/CRITIC/VALUE_FUNCTION_RBF_VARIABLES"));
	m_z= new CFeatureList();
	m_aux= new CFeatureList();

	m_pAlpha= pParameters->add("SIMGOD/CRITIC/LEARNING_RATE",0.0);
	m_gamma= pParameters->getDouble("SIMGOD/CRITIC/INITIAL_GAMMA");
	m_lambda= pParameters->getDouble("SIMGOD/CRITIC/INITIAL_LAMBDA");

	if (pParameters->exists("SIMGOD/CRITIC/LOAD"))
		loadVFunction(pParameters->getStringPtr("SIMGOD/CRITIC/LOAD"));

	if (pParameters->exists("SIMGOD/CRITIC/SAVE"))
		strcpy_s(m_saveFilename,1024,pParameters->getStringPtr("SIMGOD/CRITIC/SAVE"));
	else
		m_saveFilename[0]= 0;
}

CTDLambdaCritic::~CTDLambdaCritic()
{
	if (m_saveFilename[0]!=0)
		saveVFunction(m_saveFilename);

	delete m_pVFA;
	delete m_z;
	delete m_aux;
}

double CTDLambdaCritic::updateValue(CState *s, CAction *a, CState *s_p, double r,double rho)
{
	if (*m_pAlpha==0.0) return 0.0;
	//using sample importance
	//z= gamma * lambda * rho * z + phi_v(s)

	//not using sample importance
	//z= gamma * lambda * z + phi_v(s)
	if (g_pExperiment->m_step==1)
		m_z->clear();
	m_z->mult(m_lambda*m_gamma);//*rho);
	m_z->applyThreshold(0.0001);

	m_pVFA->getFeatures(s,0,m_aux);
	m_z->addFeatureList(m_aux,*m_pAlpha,false,true);


	//theta= theta + alpha(r + gamma*newValue - oldValue)*z
	double oldValue= m_pVFA->getValue(m_aux);

	m_pVFA->getFeatures(s_p,0,m_aux);
	double newValue= m_pVFA->getValue(m_aux);
	double td= r + m_gamma*newValue - oldValue;
	
	m_pVFA->add(m_z,td*rho);

	//m_pVFA->getFeatures(s,0,m_aux);
	//double checkOldValue= m_pVFA->getValue(m_aux);

	return td;
}

