#include "stdafx.h"
#include "vfa-critic.h"
#include "vfa.h"
#include "parameters.h"
#include "parameterized-object.h"


CVFACritic::CVFACritic(CParameters* pParameters) : CParamObject(pParameters)
{
	m_pVFA = new CLinearVFA(pParameters->getChild("Linear-VFA"));

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
