// ControllerToActorCritic.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../RLSimion-Lib/parameters.h"
#include "../RLSimion-Lib/world.h"
#include "../RLSimion-Lib/actor.h"
#include "../RLSimion-Lib/reward.h"
#include "../RLSimion-Lib/logger.h"
#include "../RLSimion-Lib/states-and-actions.h"
#include "../RLSimion-Lib/vfa.h"
#include "../RLSimion-Lib/features.h"
#include "../RLSimion-Lib/globals.h"
#include "../RLSimion-Lib/experiment.h"


CWorld *g_pWorld;
CExperiment *g_pExperiment;

int _tmain(int argc, _TCHAR* argv[])
{
	CParameters* pAppParameters;
	CParameters* pActorParameters;
	CParameters* pControllerParameters;
	CFeatureVFA **m_pVFA;
	CActor *pController;

	pAppParameters = new CParameters("ControllerToAC-parameters.txt");
	pActorParameters = new CParameters(pAppParameters->getStringPtr("ACTOR_CONFIG_FILE"));
	pControllerParameters = new CParameters(pAppParameters->getStringPtr("CONTROLLER_CONFIG_FILE"));
	g_pExperiment = new CExperiment(pControllerParameters);
	
	//INITIALISE WORLD -> STATE PROPERTIES
	g_pWorld = new CWorld(pActorParameters);

	//INTIALISE CONTROLLER: VIDAL, BOUKHEZZAR, ...
	pController = CActor::getActorInstance(pControllerParameters);

	char parameterName[MAX_PARAMETER_NAME_SIZE];

	int numOutputs= (int) pActorParameters->getDouble("SIMGOD/ACTOR/NUM_OUTPUTS");

	//INTIALISE VFA
	m_pVFA= new CFeatureVFA* [numOutputs];

	for (int i= 0; i<numOutputs; i++)
	{
		sprintf_s(parameterName,MAX_PARAMETER_NAME_SIZE,"SIMGOD/ACTOR/POLICY_RBF_VARIABLES_%d",i);
		m_pVFA[i] = new CRBFFeatureGridVFA(pActorParameters->getStringPtr(parameterName));
	}

	CState *s= g_pWorld->getStateInstance();
	CAction *a= g_pWorld->getActionInstance();


	//CONTROLLER -> ACTOR
	printf("\nSaving the weights of a VFA that approximates the controller...\n");
	FILE* pFile;
	unsigned int numWeights;
	unsigned int feature;
	double output;
	fopen_s(&pFile,pAppParameters->getStringPtr("OUTPUT_FILE"),"wb");
	if (pFile)
	{
		for (int i= 0; i<numOutputs; i++)
		{
			numWeights= m_pVFA[i]->getNumWeights();
			fwrite(&numWeights,sizeof(unsigned int),1,pFile);

			double *pWeights = new double[numWeights];
			
			for (feature= 0; feature<numWeights; feature++)
			{
				if (feature % 1000== 0)
					printf("Output dim: %d/%d      Progress: %2.2f%%\r",i+1,numOutputs,100.0*((double)feature)/((double)numWeights));
				m_pVFA[i]->getFeatureStateAction(feature,s,a);
				pController->selectAction(s,a);
				output= a->getValue(i);
				pWeights[feature] = output;
			}
			fwrite(pWeights, sizeof(double), numWeights, pFile);
			delete[] pWeights;
		}
		printf("\nDone\n");
		fclose(pFile);
	}
	else printf("ERROR: could not open output file\n");

	//CLEAN-UP
	delete a;
	delete s;
	delete g_pWorld;
	delete pController;
	delete g_pExperiment;

	for (int i= 0; i<numOutputs; i++) delete m_pVFA [i];
	delete [] m_pVFA;

	delete pAppParameters;
	delete pActorParameters;
	delete pControllerParameters;
}

