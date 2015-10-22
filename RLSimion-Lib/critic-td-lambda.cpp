#include "stdafx.h"
#include "critic.h"
#include "vfa.h"
#include "parameters.h"
#include "features.h"
#include "globals.h"
#include "experiment.h"

CTDLambdaCritic::CTDLambdaCritic(CParameters *pParameters)
{
	m_pVFA= new CRBFFeatureGridVFA(pParameters->getStringPtr("VALUE_FUNCTION_RBF_VARIABLES"));
	m_z= new CFeatureList();
	m_aux= new CFeatureList();

	m_pAlpha= g_pParameters->add("CRITIC_LEARNING_RATE",0.0);
	m_gamma= pParameters->getDouble("INITIAL_GAMMA");
	m_lambda= pParameters->getDouble("INITIAL_LAMBDA");

	if (pParameters->exists("LOAD"))
		load(pParameters->getStringPtr("LOAD"));

	if (pParameters->exists("SAVE"))
		strcpy_s(m_saveFilename,1024,pParameters->getStringPtr("SAVE"));
	else
		m_saveFilename[0]= 0;
}

CTDLambdaCritic::~CTDLambdaCritic()
{
	if (m_saveFilename[0]!=0)
		save(m_saveFilename);

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


void CTDLambdaCritic::save(char* pFilename)
{
	FILE* pFile;
	
	printf("CTDLambdaCritic::save(\"%s\")...",pFilename);

	fopen_s(&pFile,pFilename,"wb");
	if (pFile)
	{
		m_pVFA->save(pFile);
		fclose(pFile);
		printf("OK\n");
	}

}

void CTDLambdaCritic::load(char* pFilename)
{
	FILE* pFile;

	printf("CTDLambdaCritic::load(\"%s\")...",pFilename);

	fopen_s(&pFile,pFilename,"rb");
	if (pFile)
	{
		m_pVFA->load(pFile);
		fclose(pFile);

		printf("OK\n");
	}
	else printf("FAILED\n");

}