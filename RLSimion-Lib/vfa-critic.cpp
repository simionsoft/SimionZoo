#include "stdafx.h"
#include "vfa-critic.h"
#include "vfa.h"

#include "parameterized-object.h"


CVFACritic::CVFACritic(tinyxml2::XMLElement* pParameters) : CParamObject(pParameters)
{
	m_pVFA = new CLinearVFA(pParameters->FirstChildElement("Linear-VFA"));

	if (m_pParameters->FirstChildElement("LOAD"))
		loadVFunction(m_pParameters->FirstChildElement("LOAD")->GetText());
}

CVFACritic::~CVFACritic()
{
	if (m_pParameters->FirstChildElement("SAVE"))
		saveVFunction(m_pParameters->FirstChildElement("SAVE")->GetText());

	delete m_pVFA;
}

CVFACritic* CVFACritic::getInstance(tinyxml2::XMLElement* pParameters)
{
	if (!pParameters) return 0;

	tinyxml2::XMLElement* child;

	child = pParameters->FirstChildElement("TD-Lambda");
	if (child)
		return new CTDLambdaCritic(child);

	child = pParameters->FirstChildElement("True-Online-TD-Lambda");
	if (child)
		return new CTrueOnlineTDLambdaCritic(child);

	child = pParameters->FirstChildElement("TDC-Lambda");
	if (child)
		return new CTDCLambdaCritic(child);

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
