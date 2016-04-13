#include "stdafx.h"
#include "app-controller-to-vfa.h"
#include "globals.h"
#include "parameters.h"
#include "logger.h"
#include "world.h"
#include "controller.h"
#include "vfa.h"
#include "utils.h"
#include "world.h"
#include "named-var-set.h"
#include "app.h"

APP_CLASS(ControllerToVFAApp, int argc, char* argv[]) : CApp(argc,argv)
{
	try
	{
		CParameters* pParameters = m_pConfigDoc->loadFile(argv[1], "VFAToController");
		if (!pParameters) throw std::exception("Wrong experiment configuration file");
		pParameters = pParameters->getChild("VFAToController");
		if (!pParameters) throw std::exception("Wrong experiment configuration file");
		//INTIALISE CONTROLLER: VIDAL, BOUKHEZZAR, ...

		//First the world
		CDynamicModel *pWorld;
		CHILD_CLASS_FACTORY(pWorld, "Dynamic-Model", "The dynamic model's definition", false, CDynamicModel);
		//This short-cut avoids having to define all the rest of parameters of the world not used in this app (setpoints, ...)
		World.setDynamicModel(pWorld);

		CHILD_CLASS_FACTORY(m_pController, "Controller", "The controller to be approximated", false, CController);

		m_numVFAs = pParameters->countChildren("Policy");
		CParameters* pPolicyParameters = pParameters->getChild("Policy");
		m_pVFAs = new CLinearStateVFA*[m_numVFAs];
		for (int i = 0; i < m_numVFAs; i++)
		{
			MULTI_VALUED_FACTORY(m_pVFAs[i], "Policy", "The VFAs used to approximate the controller", CLinearStateVFA, pPolicyParameters);
			pPolicyParameters = pPolicyParameters->getNextChild("Policy");
		}

		CHILD_CLASS(m_pOutputDirFile, "Output-DirFile", "The output directory and file", false, CDirFileOutput);
	}
	catch (std::exception& e)
	{
		CLogger::logMessage(MessageType::Error, e.what());
	}

	END_CLASS();
}

void ControllerToVFAApp::run()
{
	try
	{
		CState *s = World.getDynamicModel()->getStateDescriptor()->getInstance();
		CAction *a = World.getDynamicModel()->getActionDescriptor()->getInstance();
		int i = 0;

		char msg[512];
		char completeFilename[1024];
		double progress;
		int outputActionIndex;
		double numDimensions = (double)std::min(m_pController->getNumOutputs(), m_numVFAs);
		for (int i = 0; i < numDimensions; i++)
		{
			outputActionIndex = m_pController->getOutputActionIndex(i);
			double numWeights = m_pVFAs[i]->getNumWeights();
			double * pWeights = m_pVFAs[i]->getWeightPtr();
			for (int feature = 0; feature < numWeights; feature++)
			{
				if (feature % 1000 == 0)
					printf("Output dim: %d      Progress: %2.2f%%\r", i + 1, 100.0*((double)feature) / ((double)numWeights));
				m_pVFAs[i]->getFeatureState(feature, s);
				m_pController->selectAction(s, a);
				double output = a->getValue(outputActionIndex);
				pWeights[feature] = output;

				progress = (((double)i) / numDimensions) + (1.0 / numDimensions) * ((double)feature) / ((double)numWeights);
				progress *= 100.0;
				sprintf_s(msg, 512, "%f", progress);
				CLogger::logMessage(MessageType::Progress, msg);
			}


			sprintf_s(completeFilename, "%s/%s.%s", m_pOutputDirFile->getOutputDir(), m_pOutputDirFile->getFilePrefix()
				, World.getDynamicModel()->getActionDescriptor()->getName(outputActionIndex));
			m_pVFAs[i]->save(completeFilename);
		}
		CLogger::logMessage(MessageType::Progress, "100");

		//CLEAN-UP
		delete a;
		delete s;
	}
	catch (std::exception& e)
	{
		CLogger::logMessage(MessageType::Error, e.what());
	}
}

ControllerToVFAApp::~ControllerToVFAApp()
{
	try
	{
		delete m_pController;
		delete m_pOutputDirFile;

		for (int i = 0; i < m_numVFAs; i++)
			if (m_pVFAs[i]) delete m_pVFAs[i];

		delete[] m_pVFAs;
	}
	catch (std::exception& e)
	{
		CLogger::logMessage(MessageType::Error, e.what());
	}
}