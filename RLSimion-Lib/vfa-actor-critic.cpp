#include "stdafx.h"
#include "vfa-actor-critic.h"
#include "vfa-actor.h"
#include "vfa.h"
#include "vfa-policy.h"
#include "parameters-xml-helper.h"

CVFAActorCritic* CVFAActorCritic::getInstance(tinyxml2::XMLElement* pParameters)
{
	if (!pParameters) return 0;

	if (!strcmp(pParameters->Name(), "INAC"))
		return new CIncrementalNaturalActorCritic(pParameters);

	return 0;
}

CVFAActorCritic::CVFAActorCritic(tinyxml2::XMLElement* pParameters) : CParamObject(pParameters)
{
	m_pVFunction = new CLinearVFA(pParameters);

	m_numOutputs = XMLParameters::countChildren(pParameters,"POLICY");
	m_pPolicies = new CSingleOutputVFAPolicy*[m_numOutputs];

	tinyxml2::XMLElement *child = pParameters->FirstChildElement("POLICY");
	for (int i = 0; i < m_numOutputs; i++)
	{
		m_pPolicies[i] = new CSingleOutputVFAPolicy(child);

		child = child->NextSiblingElement("POLICY");
	}
}

CVFAActorCritic::~CVFAActorCritic()
{
	delete m_pVFunction;
}


void CVFAActorCritic::savePolicy(const char* pFilename)
{
	FILE* pFile;

	if (!strcmp(pFilename, "NONE") || pFilename == 0 || pFilename[0] == 0) return;

	printf("Saving Policy (\"%s\")...", pFilename);

	fopen_s(&pFile, pFilename, "wb");
	if (pFile)
	{
		for (int i = 0; i<m_numOutputs; i++)
		{
			m_pPolicies[i]->getVFA()->save(pFile);
		}
		fclose(pFile);
	}

}

void CVFAActorCritic::loadPolicy(const char* pFilename)
{
	FILE* pFile;

	if (!strcmp(pFilename, "NONE") || pFilename == 0 || pFilename[0] == 0) return;

	printf("Loading Policy (\"%s\")...", pFilename);

	fopen_s(&pFile, pFilename, "rb");
	if (pFile)
	{
		for (int i = 0; i<m_numOutputs; i++)
		{
			m_pPolicies[i]->getVFA()->load(pFile);
		}
		//assert(numWeights==numWeightsRead);
		fclose(pFile);
		printf("OK\n");
	}
	else printf("FAILED\n");
}