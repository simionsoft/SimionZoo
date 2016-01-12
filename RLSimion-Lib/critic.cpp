#include "stdafx.h"
#include "critic.h"
#include "parameters.h"
#include "parameter.h"
#include "vfa.h"

CCritic* CCritic::m_pCritic = 0;

CCritic *CCritic::getCriticInstance(CParameters* pParameters)
{

	if (m_pCritic == 0 && pParameters)
	{
		const char* pAlgorithm = pParameters->getParameter("ALGORITHM")->getStringPtr();
		if (strcmp(pAlgorithm, "TD-Lambda") == 0)
			m_pCritic = new CTDLambdaCritic(pParameters);
		else if (strcmp(pAlgorithm, "True-Online-TD-Lambda") == 0)
			m_pCritic = new CTrueOnlineTDLambdaCritic(pParameters);
		else if (strcmp(pAlgorithm, "TDC-Lambda") == 0)
			m_pCritic = new CTDCLambdaCritic(pParameters);

	}
	return m_pCritic;
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

CVFACritic::CVFACritic(CParameters* pParameters)
{
	m_pVFA = new CRBFFeatureGridVFA(pParameters->getParameter("VALUE_FUNCTION_RBF_VARIABLES")->getStringPtr());

	if (pParameters->exists("LOAD"))
		loadVFunction(pParameters->getParameter("LOAD")->getStringPtr());

	if (pParameters->exists("SAVE"))
		strcpy_s(m_saveFilename, 1024, pParameters->getParameter("SAVE")->getStringPtr());
	else
		m_saveFilename[0] = 0;
}

CVFACritic::~CVFACritic()
{
	if (m_saveFilename[0] != 0)
		saveVFunction(m_saveFilename);

	delete m_pVFA;
}