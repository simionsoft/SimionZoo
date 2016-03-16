#include "stdafx.h"
#include "critic.h"
#include "vfa-critic.h"
#include "parameters.h"
#include "globals.h"
#include "logger.h"
#include "vfa.h"

CLASS_CONSTRUCTOR(CCritic) (CParameters* pParameters) : CParamObject(pParameters)
{
	CHILD_CLASS(m_pVFunction, "V-Function", CLinearStateVFA, pParameters);

	//m_pVFA = new CLinearStateVFA();
	CONST_STRING_VALUE(m_loadFile, pParameters, "Load","");
	CONST_STRING_VALUE(m_saveFile, pParameters, "Save","");
	if (m_loadFile)
		loadVFunction(m_loadFile);

	END_CLASS();
}

CCritic* CLASS_FACTORY(CCritic)(CParameters* pParameters)
{
	CHOICE("Critic");
	CHOICE_ELEMENT("TD-Lambda", CTDLambdaCritic);
	CHOICE_ELEMENT("True-Online-TD-Lambda", CTrueOnlineTDLambdaCritic);
	CHOICE_ELEMENT("TDC-Lambda", CTDCLambdaCritic);
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