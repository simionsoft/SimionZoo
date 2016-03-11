#include "stdafx.h"
#include "vfa-critic.h"
#include "vfa.h"
#include "parameters.h"
#include "parameterized-object.h"
#include "logger.h"
#include "globals.h"

CLASS_CONSTRUCTOR(CVFACritic) (CParameters* pParameters) : CParamObject(pParameters)
{
	CHILD_CLASS(m_pVFA, "V-Function", CLinearStateVFA, pParameters->getChild("V-Function"));

	//m_pVFA = new CLinearStateVFA();
	CONST_STRING_VALUE_OPTIONAL(m_loadFile, pParameters, "Load");
	CONST_STRING_VALUE_OPTIONAL(m_saveFile, pParameters, "Save");
	if (m_loadFile)
		loadVFunction(m_loadFile);

	END_CLASS();
}

CVFACritic::~CVFACritic()
{
	if (m_saveFile)
		saveVFunction(m_saveFile);

	delete m_pVFA;
}

CVFACritic* CLASS_FACTORY(CVFACritic)(CParameters* pParameters)
{
	if (!pParameters) return 0;

	CParameters* child= pParameters->getChild();

	CHOICE("Type");
	CHOICE_ELEMENT(child->getName(), "TD-Lambda", CTDLambdaCritic, child);
	CHOICE_ELEMENT(child->getName(), "True-Online-TD-Lambda", CTrueOnlineTDLambdaCritic, child);
	CHOICE_ELEMENT(child->getName(), "TDC-Lambda", CTDCLambdaCritic, child);
	CHOICE_ELEMENT(child->getName(), "Incremental-Natural-Critic", CIncrementalNaturalCritic, child);

	END_CLASS();

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


