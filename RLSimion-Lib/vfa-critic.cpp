#include "stdafx.h"
#include "vfa-critic.h"
#include "vfa.h"
#include "parameters.h"
#include "parameterized-object.h"
#include "logger.h"

CVFACritic::CVFACritic(CParameters* pParameters) : CParamObject(pParameters)
{
	m_pVFA = new CLinearStateVFA(pParameters->getChild("Linear-State-VFA"));

	if (m_pParameters->getChild("Load"))
		loadVFunction(m_pParameters->getChild("Load")->getConstString());
}

CVFACritic::~CVFACritic()
{
	if (m_pParameters->getChild("Save"))
		saveVFunction(m_pParameters->getChild("Save")->getConstString());

	delete m_pVFA;
}

CVFACritic* CVFACritic::getInstance(CParameters* pParameters)
{
	if (!pParameters) return 0;

	CParameters* child= pParameters->getChild();


	if (!strcmp(child->getName(), "TD-Lambda"))
		return new CTDLambdaCritic(child);
	if (!strcmp(child->getName(), "True-Online-TD-Lambda"))
		return new CTrueOnlineTDLambdaCritic(child);
	if (!strcmp(child->getName(), "TDC-Lambda"))
		return new CTDCLambdaCritic(child);
	if (!strcmp(child->getName(), "Incremental-Natural-Critic"))
		return new CIncrementalNaturalCritic(child);

	return 0;
}

void CVFACritic::loadVFunction(const char* pFilename)
{
	char msg[128];
	FILE* pFile;

	if (!strcmp(pFilename, "NONE") || pFilename == 0 || pFilename[0] == 0) return;

	fopen_s(&pFile, pFilename, "rb");
	if (pFile)
	{
		sprintf_s(msg, 128, "Loading Policy (\"%s\")...", pFilename);
		CLogger::logMessage(Info, msg);

		m_pVFA->load(pFile);

		//assert(numWeights==numWeightsRead);
		fclose(pFile);
		CLogger::logMessage(Info, "OK\n");
		return;
	}
	sprintf_s(msg, 128, "Loading Policy (\"%s\")...FAILED", pFilename);
	CLogger::logMessage(Warning, msg);
}

void CVFACritic::saveVFunction(const char* pFilename)
{
	char msg[128];
	FILE* pFile;

	if (!strcmp(pFilename, "NONE") || pFilename == 0 || pFilename[0] == 0) return;

	fopen_s(&pFile, pFilename, "wb");
	if (pFile)
	{
		sprintf_s(msg, 128, "Saving V-Function (\"%s\")...", pFilename);
		CLogger::logMessage(Info, msg);

		m_pVFA->save(pFile);
		
		//assert(numWeights==numWeightsRead);
		fclose(pFile);
		CLogger::logMessage(Info, "OK\n");
		return;
	}
	sprintf_s(msg, 128, "Saving V-Function (\"%s\")...FAILED", pFilename);
	CLogger::logMessage(Warning, msg);

}


