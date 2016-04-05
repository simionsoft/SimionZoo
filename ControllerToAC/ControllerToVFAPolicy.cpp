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
#include <algorithm>

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
		CLinearStateVFA* pVFAPolicy = CLinearStateVFA::getInstance(pPolicyParameters);

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
		int outputActionIndex;
		double numDimensions = (double)std::min(pController->getNumOutputs(), pPolicyParameters->countChildren());
		while (pPolicyParameters)
		{
			outputActionIndex = pController->getOutputActionIndex(i);
			double numWeights = pVFAPolicy->getNumWeights();
			double * pWeights = pVFAPolicy->getWeightPtr();
			for (int feature = 0; feature < numWeights; feature++)
			{
				if (feature % 1000 == 0)
					printf("Output dim: %d      Progress: %2.2f%%\r", i + 1, 100.0*((double)feature) / ((double)numWeights));
				pVFAPolicy->getFeatureState(feature, s);
				pController->selectAction(s, a);
				double output = a->getValue(outputActionIndex);
				pWeights[feature] = output;

				progress = (((double)i) / numDimensions) + (1.0 / numDimensions) * ((double)feature) / ((double)numWeights);
				progress *= 100.0;
				sprintf_s(msg, 512, "%f", progress);
				CLogger::logMessage(MessageType::Progress, msg);
			}

			pPolicyParameters = pPolicyParameters->getNextChild("Policy");

			sprintf_s(completeFilename, "%s/%s.%s", pOutputDirFile->getOutputDir(), pOutputDirFile->getFilePrefix(), pWorld->getActionDescriptor()->getName(outputActionIndex));
			pVFAPolicy->save(completeFilename);
			delete pVFAPolicy;
			pVFAPolicy = CLinearStateVFA::getInstance(pPolicyParameters);
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

