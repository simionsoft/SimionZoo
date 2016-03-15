#include "stdafx.h"
#include "actor.h"
#include "noise.h"
#include "controller.h"

#include "actor.h"
#include "policy.h"
#include "globals.h"
#include "named-var-set.h"
#include "parameters.h"
#include "policy-learner.h"
#include "logger.h"
#include "vfa.h"


CLASS_CONSTRUCTOR(CActor)(CParameters* pParameters) : CParamObject(pParameters)
{
	CParameters* pOutputs = pParameters->getChild("Outputs");

	m_numOutputs = pOutputs->countChildren();

	m_pPolicyLearners = new CPolicyLearner*[m_numOutputs];

	CParameters* pOutput = pOutputs->getChild();
	for (int i = 0; i<m_numOutputs; i++)
	{
		MULTI_VALUED_FACTORY(m_pPolicyLearners[i], "Outputs", CPolicyLearner, pOutput);
		//m_pPolicyLearners[i] = CVFAPolicyLearner::getInstance(pOutput);
		pOutput = pOutput->getNextChild();
	}

	CONST_STRING_VALUE(m_loadFile, pParameters, "Load","");
	CONST_STRING_VALUE(m_saveFile, pParameters, "Save","");
	if (m_loadFile) loadPolicy(m_loadFile);
	END_CLASS();
}



CActor::~CActor()
{
	if (m_saveFile) savePolicy(m_saveFile);

	for (int i = 0; i < m_numOutputs; i++)
	{
		delete m_pPolicyLearners[i];
	}

	delete[] m_pPolicyLearners;
}


void CActor::savePolicy(const char* pFilename)
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

void CActor::loadPolicy(const char* pFilename)
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

void CActor::selectAction(const CState *s, CAction *a)
{
	for (int i = 0; i<m_numOutputs; i++)
	{
		//each uni-dimensional policy sets its own action's value
		m_pPolicyLearners[i]->getPolicy()->selectAction(s, a);
	}
}

void CActor::updatePolicy(const CState* s, const CAction* a, const CState* s_p, double r, double td)
{
	for (int i = 0; i<m_numOutputs; i++)
	{
		//each uni-dimensional policy sets its own action's value
		m_pPolicyLearners[i]->updatePolicy(s, a, s_p, r, td);
	}
}