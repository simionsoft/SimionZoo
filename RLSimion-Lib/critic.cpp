#include "stdafx.h"
#include "critic.h"
#include "vfa-critic.h"
#include "parameters.h"
#include "globals.h"
#include "logger.h"
#include "vfa.h"

CCritic* CLASS_FACTORY(CCritic)(CParameters* pParameters)
{
	if (!pParameters) return 0;

	CParameters* pChild = pParameters->getChild();
	const char* type = pChild->getName();

	CHOICE("Type");
	CHOICE_ELEMENT(type, "TD-Lambda", CTDLambdaCritic, pChild);
	CHOICE_ELEMENT(type, "True-Online-TD-Lambda", CTrueOnlineTDLambdaCritic, pChild);
	CHOICE_ELEMENT(type, "TDC-Lambda", CTDCLambdaCritic, pChild);
	//CHOICE-ELEMENT-COMMENTED(type, "Incremental-Natural-Critic", CIncrementalNaturalCritic, pChild);
	END_CHOICE();

	END_CLASS();

	return 0;
}

CCritic::~CCritic()
{
	if (m_saveFile)
		saveVFunction(m_saveFile);

	delete m_pVFunction;
}


void CCritic::loadVFunction(const char* pFilename)
{
	char msg[128];
	FILE* pFile;

	if (!strcmp(pFilename, "NONE") || pFilename == 0 || pFilename[0] == 0) return;

	fopen_s(&pFile, pFilename, "rb");
	if (pFile)
	{
		sprintf_s(msg, 128, "Loading Policy (\"%s\")...", pFilename);
		CLogger::logMessage(Info, msg);

		m_pVFunction->load(pFile);

		//assert(numWeights==numWeightsRead);
		fclose(pFile);
		CLogger::logMessage(Info, "OK\n");
		return;
	}
	sprintf_s(msg, 128, "Loading Policy (\"%s\")...FAILED", pFilename);
	CLogger::logMessage(Warning, msg);
}

void CCritic::saveVFunction(const char* pFilename)
{
	char msg[128];
	FILE* pFile;

	if (!strcmp(pFilename, "NONE") || pFilename == 0 || pFilename[0] == 0) return;

	fopen_s(&pFile, pFilename, "wb");
	if (pFile)
	{
		sprintf_s(msg, 128, "Saving V-Function (\"%s\")...", pFilename);
		CLogger::logMessage(Info, msg);

		m_pVFunction->save(pFile);

		//assert(numWeights==numWeightsRead);
		fclose(pFile);
		CLogger::logMessage(Info, "OK\n");
		return;
	}
	sprintf_s(msg, 128, "Saving V-Function (\"%s\")...FAILED", pFilename);
	CLogger::logMessage(Warning, msg);

}