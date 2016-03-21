// ControllerToActorCritic.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../RLSimion-Lib/parameters.h"
#include "../RLSimion-Lib/world.h"
#include "../RLSimion-Lib/actor.h"
#include "../RLSimion-Lib/reward.h"
#include "../RLSimion-Lib/logger.h"
#include "../RLSimion-Lib/named-var-set.h"
#include "../RLSimion-Lib/vfa.h"
#include "../RLSimion-Lib/features.h"
#include "../RLSimion-Lib/globals.h"
#include "../RLSimion-Lib/experiment.h"
#include "../RLSimion-Lib/policy.h"
#include "../RLSimion-Lib/controller.h"

int main(int argc, char* argv[])
{
	CParameters* pConfig= RLSimion::init(argc, argv, "Controller-To-VFA-Policy");


	//INTIALISE CONTROLLER: VIDAL, BOUKHEZZAR, ...
	CController* pController = CController::getInstance(pConfig->getChild("Controller"));

	CDynamicModel* pWorld= CDynamicModel::getInstance(pConfig->getChild("World"));


	CParameters* pPolicyParameters = pConfig->getChild("Policy");
	CDeterministicPolicy* pVFAPolicy = CDeterministicPolicy::getInstance(pPolicyParameters);


	if (!pController || !pWorld || !pVFAPolicy)
	{
		printf("Configuration error: Some component is missing in the config file\n");
		exit(-1);
	}

	CState *s = pWorld->getStateDescriptor();
	CAction *a = pWorld->getActionDescriptor();
	int i = 0;

	while (pPolicyParameters)
	{
		double numWeights = pVFAPolicy->getVFA()->getNumWeights();
		double * pWeights = pVFAPolicy->getVFA()->getWeightPtr();
		for (int feature = 0; feature < numWeights; feature++)
		{
			if (feature % 1000 == 0)
				printf("Output dim: %d      Progress: %2.2f%%\r", i + 1, 100.0*((double)feature) / ((double)numWeights));
			pVFAPolicy->getVFA()->getFeatureState(feature, s);
			pController->selectAction(s, a);
			double output = a->getValue(feature);
			pWeights[feature] = output;
		}
		pPolicyParameters = pPolicyParameters->getNextChild("Policy");
		delete pVFAPolicy;
		pVFAPolicy = CDeterministicPolicy::getInstance(pPolicyParameters);
		i++;
	}

	//CLEAN-UP
	delete a;
	delete s;
	RLSimion::shutdown();
	delete pController;
	delete pVFAPolicy;
	delete pWorld;


	//CParameters* pSingleOutputVFAConfig = pConfig->getChild("Single-Output-VFA");



	////CONTROLLER -> ACTOR
	//printf("\nSaving the weights of a VFA that approximates the controller...\n");
	//FILE *pBinFile, *pXMLFile;
	//unsigned int numWeights;
	//unsigned int feature;
	//double output;
	//char binFilename[512];
	//char xmlFilename[512];

	//sprintf_s(binFilename, 512, "%s.weights.bin", pOutputConfig->getConstString());
	//sprintf_s(xmlFilename, 512, "%s.feature-map.xml", pOutputConfig->getConstString());
	//fopen_s(&pBinFile, pOutputConfig->getConstString(), "wb");
	//
	//int i = 0;
	//if (pBinFile)
	//{
	//	fopen_s(&pXMLFile,xmlFilename, "w");
	//	if (pXMLFile)
	//	{
	//		CLinearStateVFA* pVFA;
	//		CParameters* pFeatureMapParameters;
	//		const char* pOutputAction;

	//		while (pSingleOutputVFAConfig)
	//		{
	//			pVFA = new CLinearStateVFA(pSingleOutputVFAConfig->getChild("Linear-State-VFA"));
	//			pFeatureMapParameters = pVFA->getParameters();
	//			pOutputAction = pSingleOutputVFAConfig->getChild("Output-Action")->getConstString();

	//			numWeights = pVFA->getNumWeights();
	//			fwrite(&numWeights, sizeof(unsigned int), 1, pBinFile);

	//			double *pWeights = new double[numWeights];

	//			for (feature = 0; feature < numWeights; feature++)
	//			{
	//				if (feature % 1000 == 0)
	//					printf("Output dim: %d      Progress: %2.2f%%\r", i + 1, 100.0*((double)feature) / ((double)numWeights));
	//				pVFA->getFeatureState(feature, s);
	//				pController->selectAction(s, a);
	//				output = a->getValue(i);
	//				pWeights[feature] = output;
	//			}
	//			pFeatureMapParameters->saveFile(pXMLFile);
	//			fwrite(pWeights, sizeof(double), numWeights, pBinFile);
	//			delete[] pWeights;
	//			delete pVFA;
	//			i++;

	//			pSingleOutputVFAConfig = pSingleOutputVFAConfig->getNextChild("Single-Output-VFA");
	//		}
	//		fclose(pXMLFile);
	//	}
	//	printf("\nDone\n");
	//	fclose(pBinFile);
	//}
	//else printf("ERROR: could not open output file\n");
	



}

