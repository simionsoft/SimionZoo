#include "stdafx.h"
#include "actor.h"
#include "vfa.h"
#include "noise.h"
#include "globals.h"
#include "world.h"
#include "states-and-actions.h"
#include "parameters.h"
#include "parameter.h"
#include "features.h"
#include "experiment.h"

CCACLAActor::CCACLAActor(CParameters *pParameters) : CVFAActor(pParameters)
{

}

CCACLAActor::~CCACLAActor()
{

}

void CVFAActor::selectAction(CState *s,CAction *a)
{
	double a_width;
	double noise;
	double u_i;
	double prob= 1.0;

	//if (m_outputTime == g_pExperiment->m_expProgress)
	//{
	//	a->copy(m_pOutput);
	//}
	//else
	{
		for (int i= 0; i<m_numOutputs; i++)
		{
			a_width= 0.5*(a->getMax(i) - a->getMin(i));
			noise= m_pExpNoise[i]->getNewValue() * a_width;

			m_pPolicy[i]->getFeatures(s,0,m_pStateFeatures);

			u_i= m_pPolicy[i]->getValue(m_pStateFeatures);

			a->setValue(i, u_i + noise);
		}
		//m_pOutput->copy(a);
		//m_outputTime = g_pExperiment->m_expProgress;
	}


}


double CVFAActor::getProbability(CState* s, CAction* a)
{
	double actionProb= 1.0;
	double actionDist= 0.0;
	double mahalanobisDist= 0.0;
	double varProd= 1.0;
	double noiseWidth;
	double var_i; //action's i-th dimension's variance
	double a_width;


	//if (g_pExperiment->isEvaluationEpisode())
	//	return 1.0;

	//for (int i = 0; i < m_numOutputs; i++)
	//{
	//	varProd *= (1 - std::max(1.0, fabs(m_pExpNoise[i]->getLastValue())));
	//}
	//return varProd;


	//http://en.wikipedia.org/wiki/Multivariate_normal_distribution

	//if there is no correlation:
	//f(x,mu)= exp(-0.5 * (x-mu)^T * (x-mu)/var(x))/ sqrt(2*pi^k* prod var(x))
	selectAction(s, m_pOutput);
	for (int i = 0; i<m_numOutputs ;i++)
	{
		var_i= std::max(0.000000001,m_pExpNoise[i]->getSigma());
		a_width = 0.5*(a->getMax(i) - a->getMin(i));
		noiseWidth = fabs((a->getValue(i) - m_pOutput->getValue(i)) / a_width);

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

	return std::min(1.0,expo/root);
}

void CCACLAActor::updatePolicy(CState *s,CAction *a,CState *s_p,double r,double td)
{
	double lastNoise;
	/*double a_width;*/

	//CACLA (van Hasselt)
	//if delta>0: theta= theta + alpha*(lastNoise)*phi_pi(s)

	if (td>0 && !g_pExperiment->isEvaluationEpisode())
	{
		//MAYBE WE CAN CHECK WHETHER WE HAVE ALREADY CALCULATED THE OUTPUT OF THE CONTROLLER
		selectAction(s, m_pOutput);

		for (int i= 0; i<m_numOutputs; i++)
		{
			lastNoise = a->getValue(i) - m_pOutput->getValue(i);

			m_pPolicy[i]->getFeatures(s,0,m_pStateFeatures);

			if (m_pAlpha[i]->getDouble() != 0.0)
				m_pPolicy[i]->add(m_pStateFeatures,m_pAlpha[i]->getDouble()*lastNoise);
		}
	}
}


void CVFAActor::savePolicy(const char* pFilename)
{
	FILE* pFile;

	if (!strcmp(pFilename, "NONE")) return;

	printf("Saving Policy (\"%s\")...", pFilename);

	fopen_s(&pFile, pFilename, "wb");
	if (pFile)
	{
		for (int i = 0; i<m_numOutputs; i++)
		{
			m_pPolicy[i]->save(pFile);
		}
		fclose(pFile);
	}

}

void CVFAActor::loadPolicy(const char* pFilename)
{
	FILE* pFile;

	if (!strcmp(pFilename, "NONE")) return;

	printf("Loading Policy (\"%s\")...", pFilename);

	fopen_s(&pFile, pFilename, "rb");
	if (pFile)
	{
		for (int i = 0; i<m_numOutputs; i++)
		{
			m_pPolicy[i]->load(pFile);
		}
		//assert(numWeights==numWeightsRead);
		fclose(pFile);
		printf("OK\n");
	}
	else printf("FAILED\n");
}