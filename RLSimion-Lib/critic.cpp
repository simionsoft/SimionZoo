#include "stdafx.h"
#include "critic.h"
#include "parameters.h"
#include "vfa.h"

CCritic* CCritic::m_pCritic = 0;

CCritic *CCritic::getCriticInstance(CParameters* pParameters)
{

	if (m_pCritic == 0 && pParameters)
	{

		if (strcmp(pParameters->getStringPtr("SIMGOD/CRITIC/ALGORITHM"), "TD-Lambda") == 0)
			m_pCritic = new CTDLambdaCritic(pParameters);
		else if (strcmp(pParameters->getStringPtr("SIMGOD/CRITIC/ALGORITHM"), "True-Online-TD-Lambda") == 0)
			m_pCritic = new CTrueOnlineTDLambdaCritic(pParameters);
		else if (strcmp(pParameters->getStringPtr("SIMGOD/CRITIC/ALGORITHM"), "TDC-Lambda") == 0)
			m_pCritic = new CTDCLambdaCritic(pParameters);

	}
	return m_pCritic;
}


void CCritic::saveVFunction(char* pFilename)
{
	FILE* pFile;

	printf("Saving Value-Function (\"%s\")...", pFilename);

	fopen_s(&pFile, pFilename, "wb");
	if (pFile)
	{
		m_pVFA->save(pFile);
		fclose(pFile);
		printf("OK\n");
	}

}

void CCritic::loadVFunction(char* pFilename)
{
	FILE* pFile;

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