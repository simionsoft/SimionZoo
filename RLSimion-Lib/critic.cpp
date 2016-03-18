#include "stdafx.h"
#include "critic.h"
#include "vfa-critic.h"
#include "parameters.h"
#include "globals.h"
#include "logger.h"
#include "vfa.h"

CLASS_CONSTRUCTOR(CCritic) : CParamObject(pParameters)
{
	CHILD_CLASS(m_pVFunction, "V-Function","The parameterization of the V-Function to be learned","", CLinearStateVFA);

	//m_pVFA = new CLinearStateVFA();
	CONST_STRING_VALUE(m_loadFile, "Load","","File from where the weights of the V-Function will be loaded before the experiment");
	CONST_STRING_VALUE(m_saveFile, "Save", "", "File where the weights of the V-Function will be saved after the experiment");
	if (m_loadFile)
		loadVFunction(m_loadFile);

	END_CLASS();
}

CLASS_FACTORY(CCritic)
{
	CHOICE("Critic","Critic type");
	CHOICE_ELEMENT("TD-Lambda", CTDLambdaCritic,"TD-Lambda algorithm");
	CHOICE_ELEMENT("True-Online-TD-Lambda", CTrueOnlineTDLambdaCritic,"True-online TD-Lambda algorithm");
	CHOICE_ELEMENT("TDC-Lambda", CTDCLambdaCritic,"TDC-Lambda algorithm");
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