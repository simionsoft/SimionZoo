#include "stdafx.h"
#include "actor.h"
#include "parameters.h"
#include "controller.h"
#include "vfa.h"

CActor* CActor::m_pActor = 0;

CActor *CActor::getActorInstance(CParameters* pParameters)
{

	if (m_pActor == 0 && pParameters)
	{
		if (strcmp(pParameters->getStringPtr("SIMGOD/ACTOR/ALGORITHM"), "CACLA") == 0)
		m_pActor = new CCACLAActor(pParameters);
		else if (strcmp(pParameters->getStringPtr("SIMGOD/ACTOR/ALGORITHM"), "VIDAL") == 0)
			m_pActor = new CWindTurbineVidalController(pParameters);
		else if (strcmp(pParameters->getStringPtr("SIMGOD/ACTOR/ALGORITHM"), "BOUKHEZZAR") == 0)
			m_pActor = new CWindTurbineBoukhezzarController(pParameters);
		else if (strcmp(pParameters->getStringPtr("SIMGOD/ACTOR/ALGORITHM"), "PID") == 0)
			m_pActor = new CPIDController(pParameters);
		else if (strcmp(pParameters->getStringPtr("SIMGOD/ACTOR/ALGORITHM"), "LQR") == 0)
			m_pActor = new CLQRController(pParameters);
	}

	return m_pActor;
}


void CActor::savePolicy(char* pFilename)
{
	FILE* pFile;

	printf("CACLAActor::load(\"%s\")...", pFilename);

	fopen_s(&pFile, pFilename, "wb");
	if (pFile)
	{
		for (int i = 0; i<m_numOutputs; i++)
		{
			m_pPolicy[i]->save(pFile);
		}
		fclose(pFile);
	}

}

void CActor::loadPolicy(char* pFilename)
{
	FILE* pFile;

	printf("CACLAActor::load(\"%s\")...", pFilename);

	fopen_s(&pFile, pFilename, "rb");
	if (pFile)
	{
		for (int i = 0; i<m_numOutputs; i++)
		{
			m_pPolicy[i]->load(pFile);
		}
		//assert(numWeights==numWeightsRead);
		fclose(pFile);
		printf("OK\n");
	}
	else printf("FAILED\n");

}