#include <stdafx.h>
#include "actor.h"
#include "vfa.h"
#include "noise.h"
#include "globals.h"
#include "world.h"
#include "states-and-actions.h"
#include "parameters.h"
#include "features.h"
#include "experiment.h"

CCACLAActor::CCACLAActor(CParameters *pParameters)
{
	char parameterName[MAX_PARAMETER_NAME_SIZE];

	m_numOutputs= (int) pParameters->getDouble("NUM_OUTPUTS");

	m_pAlpha= new double* [m_numOutputs];
	m_pPolicy= new CFeatureVFA* [m_numOutputs];
	m_pExpNoise= new CGaussianNoise* [m_numOutputs];

	for (int i= 0; i<m_numOutputs; i++)
	{
		sprintf_s(parameterName,MAX_PARAMETER_NAME_SIZE,"ACTOR_LEARNING_RATE_%d",i);
		m_pAlpha[i]= g_pParameters->add(parameterName,0.0);

		sprintf_s(parameterName,MAX_PARAMETER_NAME_SIZE,"POLICY_RBF_VARIABLES_%d",i);
		m_pPolicy[i]= new CRBFFeatureGridVFA(pParameters->getStringPtr(parameterName));

		m_pExpNoise[i]= new CGaussianNoise(i);
	}

	m_pStateFeatures= new CFeatureList();

	if (pParameters->exists("LOAD"))
		load(pParameters->getStringPtr("LOAD"));

	if (pParameters->exists("SAVE"))
		strcpy_s(m_saveFilename,1024,pParameters->getStringPtr("SAVE"));
	else
		m_saveFilename[0]= 0;

}

CCACLAActor::~CCACLAActor()
{
	if (m_saveFilename[0]!=0)
		save(m_saveFilename);

	for (int i= 0; i<m_numOutputs; i++)
	{
		delete m_pAlpha[i];
		delete m_pPolicy[i];
		delete m_pExpNoise[i];
	}
	delete [] m_pAlpha;
	delete [] m_pPolicy;
	delete [] m_pExpNoise;

	delete m_pStateFeatures;
}

double CCACLAActor::selectAction(CState *s,CAction *a)
{
	double a_width;
	double noise;
	double u_i;
	double prob= 1.0;

	for (int i= 0; i<m_numOutputs; i++)
	{
		a_width= 0.5*(a->getMax(i) - a->getMin(i));
		noise= m_pExpNoise[i]->getNewValue() * a_width;

		m_pPolicy[i]->getFeatures(s,0,m_pStateFeatures);

		u_i= m_pPolicy[i]->getValue(m_pStateFeatures);

		a->setValue(i, u_i + noise);

		//if (!g_pExperiment->isEvaluationEpisode())
		//	prob*= m_pExpNoise[i]->getLastValuesProbability();
	}

	return getProbability(a);
		//prob;
}


double CCACLAActor::getProbability(CAction* a)
{
	double actionProb= 1.0;
	double actionDist= 0.0;
	double mahalanobisDist= 0.0;
	double varProd= 1.0;
	double noiseWidth;
	double var_i; //action's i-th dimension's variance

	if (g_pExperiment->isEvaluationEpisode())
		return 1.0;


	//http://en.wikipedia.org/wiki/Multivariate_normal_distribution

	//if there is no correlation:
	//f(x,mu)= exp(-0.5 * (x-mu)^T * (x-mu)/var(x))/ sqrt(2*pi^k* prod var(x))
	for (int i = 0; i<m_numOutputs ;i++)
	{
		var_i= m_pExpNoise[i]->getSigma();
		noiseWidth= m_pExpNoise[i]->getLastValue();

		if (noiseWidth!=0.0)
		{
			//actionDist= (m_pExpNoise[i]->getLastValue()/noiseWidth)*3.0; //because we are using sigma=1 /3 to map values into [-1,1]
			mahalanobisDist+= noiseWidth*noiseWidth / (var_i*var_i);// Variance=1.0 , otherwise it should be  / (var_i*var_i);
			varProd= varProd* var_i*var_i;
		}
	}

	if (mahalanobisDist==0.0) return 1.0;

	double root= sqrt(pow((2*M_PI),(double)m_numOutputs)*varProd);
	double expo= exp(-0.5*mahalanobisDist);

	return min(1.0,expo/root);
}

void CCACLAActor::update(CState *s,CAction *a,CState *s_p,double r,double td)
{
	double lastNoise;
	double a_width;

	//if delta>0: theta= theta + alpha*(lastNoise)*phi_pi(s)
	if (td>0 && !g_pExperiment->isEvaluationEpisode())
	{
		for (int i= 0; i<m_numOutputs; i++)
		{
			a_width= 0.5*(a->getMax(i) - a->getMin(i));
			lastNoise= m_pExpNoise[i]->getLastValue() * a_width;

			m_pPolicy[i]->getFeatures(s,0,m_pStateFeatures);

			if (*(m_pAlpha[i]) != 0.0)
				m_pPolicy[i]->add(m_pStateFeatures,(*(m_pAlpha[i]))*lastNoise);
		}
	}
}

void CCACLAActor::save(char* pFilename)
{
	FILE* pFile;

	printf("CACLAActor::load(\"%s\")...",pFilename);

	fopen_s(&pFile,pFilename,"wb");
	if (pFile)
	{
		for (int i= 0; i<m_numOutputs; i++)
		{
			m_pPolicy[i]->save(pFile);
		}
		fclose(pFile);
	}

}

void CCACLAActor::load(char* pFilename)
{
	FILE* pFile;

	printf("CACLAActor::load(\"%s\")...",pFilename);

	fopen_s(&pFile,pFilename,"rb");
	if (pFile)
	{		
		for (int i= 0; i<m_numOutputs; i++)
		{
			m_pPolicy[i]->load(pFile);
		}
		//assert(numWeights==numWeightsRead);
		fclose(pFile);
		printf("OK\n");
	}
	else printf("FAILED\n");

}