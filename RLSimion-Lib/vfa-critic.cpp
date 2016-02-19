#include "stdafx.h"
#include "vfa-critic.h"
#include "vfa.h"

#include "parameterized-object.h"


CVFACritic::CVFACritic(tinyxml2::XMLElement* pParameters) : CParamObject(pParameters)
{
	m_pVFA = new CLinearVFA(pParameters->FirstChildElement("Linear-VFA"));

	if (m_pParameters->FirstChildElement("Load"))
		loadVFunction(m_pParameters->FirstChildElement("Load")->GetText());
}

CVFACritic::~CVFACritic()
{
	if (m_pParameters->FirstChildElement("Save"))
		saveVFunction(m_pParameters->FirstChildElement("Save")->GetText());

	delete m_pVFA;
}

CVFACritic* CVFACritic::getInstance(tinyxml2::XMLElement* pParameters)
{
	if (!pParameters) return 0;

	tinyxml2::XMLElement* child= pParameters->FirstChildElement();


	if (!strcmp(child->Name(), "TD-Lambda"))
		return new CTDLambdaCritic(child);
	if (!strcmp(child->Name(), "True-Online-TD-Lambda"))
		return new CTrueOnlineTDLambdaCritic(child);
	if (!strcmp(child->Name(), "TDC-Lambda"))
		return new CTDCLambdaCritic(child);
	if (!strcmp(child->Name(), "Incremental-Natural-Critic"))
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
