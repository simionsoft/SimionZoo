#include "stdafx.h"
#include "named-var-set.h"
#include "noise.h"
#include "vfa-actor.h"
#include "vfa.h"
#include "vfa-policy.h"
#include "actor.h"
#include "globals.h"
#include "parameters.h"
#include "logger.h"

CVFAPolicyLearner::CVFAPolicyLearner(CParameters* pParameters) : CParamObject(pParameters)
{
	m_pPolicy = CDeterministicVFAPolicy::getInstance(pParameters->getChild("Deterministic-VFA-Policy"));
}

CVFAPolicyLearner::~CVFAPolicyLearner()
{
	delete m_pPolicy;
}

CVFAPolicyLearner* CVFAPolicyLearner::getInstance(CParameters* pParameters)
{
	if (pParameters)
	{
		if (!strcmp(pParameters->getName(), "CACLA"))
			return new CCACLALearner(pParameters);
		if (!strcmp(pParameters->getName(), "Regular-Gradient"))
			return new CRegularPolicyGradientLearner(pParameters);
		if (!strcmp(pParameters->getName(), "Incremental-Natural-Actor"))
			return new CIncrementalNaturalActor(pParameters);
	}
	return 0;
}

CVFAActor::CVFAActor(CParameters* pParameters): CParamObject(pParameters)
{
	CParameters* pOutputs = pParameters->getChild("Outputs");

	m_numOutputs = pOutputs->countChildren();
	
	m_pPolicyLearners = new CVFAPolicyLearner*[m_numOutputs];

	CParameters* pOutput= pOutputs->getChild();
	for (int i = 0; i<m_numOutputs; i++)
	{
		m_pPolicyLearners[i] = CVFAPolicyLearner::getInstance(pOutput);
		pOutput = pOutput->getNextChild();
	}

	if (pParameters->getChild("Load"))
		loadPolicy(pParameters->getChild("Load")->getConstString());

}

CVFAActor::~CVFAActor()
{
	if (m_pParameters->getChild("Save"))
		savePolicy(m_pParameters->getChild("Save")->getConstString());

	for (int i = 0; i<m_numOutputs; i++)
	{
		delete m_pPolicyLearners[i];
	}

	delete [] m_pPolicyLearners;

}

void CVFAActor::selectAction(const CState *s, CAction *a)
{
	for (int i = 0; i<m_numOutputs; i++)
	{
		//each uni-dimensional policy sets its own action's value
		m_pPolicyLearners[i]->getPolicy()->selectAction(s, a);
	}
}

void CVFAActor::updatePolicy(const CState* s, const CAction* a, const CState* s_p, double r, double td)
{
	for (int i = 0; i<m_numOutputs; i++)
	{
		//each uni-dimensional policy sets its own action's value
		m_pPolicyLearners[i]->updatePolicy(s, a, s_p, r, td);
	}
}

/*

//doesn't work, not sure if it should either

double CVFAActor::getProbability(CState* s, CAction* a)
{
	double actionProb = 1.0;
	double actionDist = 0.0;
	double mahalanobisDist = 0.0;
	double varProd = 1.0;
	double noiseWidth;
	double var_i; //action's i-th dimension's variance
	double output;

	if (RLSimion::g_pExperiment->isEvaluationEpisode())
		return 1.0;
	//http://en.wikipedia.org/wiki/Multivariate_normal_distribution

	//if there is no correlation:
	//f(x,mu)= exp(-0.5 * (x-mu)^T * (x-mu)/var(x))/ sqrt(2*pi^k* prod var(x))

	for (int i = 0; i<m_numOutputs; i++)
	{
		output = m_pPolicyLearners[i]->getPolicy()->getVFA()->getValue(s, a);

		var_i = std::max(0.000000001, m_pPolicyLearners[i]->getPolicy()->getExpNoise()->getSigma());

		noiseWidth = fabs((a->getValue(i) - output) 
			/ a->getRangeHalfWidth(m_pPolicyLearners[i]->getPolicy()->getOutputActionIndex()));

		if (noiseWidth != 0.0)
		{
			//actionDist= (m_pExpNoise[i]->getLastValue()/noiseWidth)*3.0; //because we are using sigma=1 /3 to map values into [-1,1]
			mahalanobisDist += noiseWidth*noiseWidth / (var_i*var_i);// Variance=1.0 , otherwise it should be  / (var_i*var_i);
			varProd = varProd* var_i*var_i;
		}
	}

	if (mahalanobisDist == 0.0) return 1.0;

	double root = sqrt(pow((2 * M_PI), (double)m_numOutputs)*varProd);
	double expo = exp(-0.5*mahalanobisDist);

	return std::min(1.0, expo / root);
}
*/

void CVFAActor::savePolicy(const char* pFilename)
{
	CParameters* pFeatureMapParameters;
	char msg[128];
	char binFile[512];
	char xmlDescFile[512];

	FILE* pBinFile;
	FILE* pXMLFile;

	if (!strcmp(pFilename, "NONE") || pFilename == 0 || pFilename[0] == 0) return;

	sprintf_s(binFile, 512, "%s.weights.bin", pFilename);
	sprintf_s(xmlDescFile, 512, "%s.feature-map.xml", pFilename);
	fopen_s(&pBinFile, binFile, "wb");//////////NOT CALLED!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	if (pBinFile)
	{
		fopen_s(&pXMLFile, xmlDescFile, "w");
		if (pXMLFile)
		{
			sprintf_s(msg, 128, "Saving Policy (\"%s\" (.bin/.xml)...", pFilename);
			CLogger::logMessage(Info, msg);

			for (int i = 0; i < m_numOutputs; i++)
			{
				pFeatureMapParameters = m_pPolicyLearners[i]->getPolicy()->getVFA()->getParameters();
				if (pFeatureMapParameters)
					pFeatureMapParameters->saveFile(pXMLFile);
				m_pPolicyLearners[i]->getPolicy()->getVFA()->save(pBinFile);
			}
			fclose(pBinFile);
			fclose(pXMLFile);
			CLogger::logMessage(Info, "OK\n");
			return;
		}
	}
	sprintf_s(msg, 128, "Saving Policy (\"%s\")...FAILED", binFile);
	CLogger::logMessage(Warning, msg);

}

void CVFAActor::loadPolicy(const char* pFilename)
{
	char msg[128];
	FILE* pFile;

	if (!strcmp(pFilename, "NONE") || pFilename == 0 || pFilename[0] == 0) return;

	fopen_s(&pFile, pFilename, "rb");
	if (pFile)
	{
		sprintf_s(msg, 128, "Loading Policy (\"%s\")...", pFilename);
		CLogger::logMessage(Info, msg);

		for (int i = 0; i<m_numOutputs; i++)
		{
			m_pPolicyLearners[i]->getPolicy()->getVFA()->load(pFile);
		}
		//assert(numWeights==numWeightsRead);
		fclose(pFile);
		CLogger::logMessage(Info, "OK\n");
		return;
	}
	sprintf_s(msg, 128, "Loading Policy (\"%s\")...FAILED", pFilename);
	CLogger::logMessage(Warning, msg);
}