#include "stdafx.h"
#include "vfa-critic.h"
#include "vfa.h"
#include "parameters.h"
#include "parameter.h"
#include "parameterized-object.h"


CVFACritic::CVFACritic(CParameters* pParameters) : CParamObject(pParameters)
{
	m_pVFA = new CLinearVFA(pParameters->getChild("VFA"));

	if (m_pParameters->exists("LOAD"))
		loadVFunction(m_pParameters->getParameter("LOAD")->getStringPtr());
}

CVFACritic::~CVFACritic()
{
	if (m_pParameters->exists("SAVE"))//(m_saveFilename[0] != 0)
		saveVFunction(m_pParameters->getParameter("SAVE")->getStringPtr());

	delete m_pVFA;
}

CVFACritic* CVFACritic::getInstance(CParameters* pParameters)
{
	if (!pParameters) return 0;

	const char* pLearner = pParameters->getName();

	if (strcmp(pLearner, "TD-Lambda") == 0)
		return new CTDLambdaCritic(pParameters);
	else if (strcmp(pLearner, "True-Online-TD-Lambda") == 0)
		return new CTrueOnlineTDLambdaCritic(pParameters);
	else if (strcmp(pLearner, "TDC-Lambda") == 0)
		return new CTDCLambdaCritic(pParameters);

	return 0;
}


void CVFACritic::saveVFunction(const char* pFilename)
{
	FILE* pFile;

	if (!strcmp("NONE", pFilename)) return;

	printf("Saving Value-Function (\"%s\")...", pFilename);

	fopen_s(&pFile, pFilename, "wb");
	if (pFile)
	{
		m_pVFA->save(pFile);
		fclose(pFile);
		printf("OK\n");
	}

}

void CVFACritic::loadVFunction(const char* pFilename)
{
	FILE* pFile;

	if (!strcmp(pFilename, "NONE")) return;

	printf("Loading Value-Function (\"%s\")...", pFilename);

	fopen_s(&pFile, pFilename, "rb");
	if (pFile)
	{
		m_pVFA->load(pFile);
		fclose(pFile);

		printf("OK\n");
	}
	else printf("FAILED\n");

}
