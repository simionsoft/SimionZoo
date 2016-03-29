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
#include "../RLSimion-Lib/utils.h"

int main(int argc, char* argv[])
{
	try
	{
		CParameters* pConfig = RLSimion::init(argc, argv, "ControllerToVFAPolicy");


		//INTIALISE CONTROLLER: VIDAL, BOUKHEZZAR, ...

		//First the world
		CDynamicModel *pWorld = CDynamicModel::getInstance(pConfig->getChild("Dynamic-Model"));
		RLSimion::World.setDynamicModel(pWorld);

		CController* pController = CController::getInstance(pConfig->getChild("Controller"));

		CParameters* pPolicyParameters = pConfig->getChild("Policy");
		CDeterministicPolicy* pVFAPolicy = new CDeterministicPolicy(pPolicyParameters);

		CDirFileOutput* pOutputDirFile = new CDirFileOutput(pConfig->getChild("Output-DirFile"));

		if (!pController || !pWorld || !pVFAPolicy || !pOutputDirFile)
		{
			printf("Configuration error: Some component is missing in the config file\n");
			exit(-1);
		}

		CState *s = pWorld->getStateInstance();
		CAction *a = pWorld->getActionInstance();
		int i = 0;

		char msg[512];
		char completeFilename[1024];
		double progress;
		double numDimensions = (double)pPolicyParameters->countChildren();
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
				double output = a->getValue(pVFAPolicy->getOutputActionIndex());
				pWeights[feature] = output;

				progress = (((double)i) / numDimensions) + (1.0 / numDimensions) * ((double)feature) / ((double)numWeights);
				progress *= 100.0;
				sprintf_s(msg, 512, "%f", progress);
				CLogger::logMessage(MessageType::Progress, msg);
			}

			pPolicyParameters = pPolicyParameters->getNextChild("Policy");

			sprintf_s(completeFilename, "%s/%s.%s", pOutputDirFile->getOutputDir(), pOutputDirFile->getFilePrefix(), pVFAPolicy->getParameters()->getConstString("Output-Action"));
			pVFAPolicy->getVFA()->save(completeFilename);
			delete pVFAPolicy;
			pVFAPolicy = CDeterministicPolicy::getInstance(pPolicyParameters);
			i++;
		}
		CLogger::logMessage(MessageType::Progress, "100");

		//CLEAN-UP
		delete a;
		delete s;
		RLSimion::shutdown();
		delete pController;
		delete pVFAPolicy;
	}
	catch (std::exception& e)
	{
		CLogger::logMessage(MessageType::Error, e.what());
	}

}

