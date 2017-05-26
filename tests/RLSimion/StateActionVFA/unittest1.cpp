#include "stdafx.h"
#include "CppUnitTest.h"
#include "../../../RLSimion-Lib/vfa.h"
#include "../../../RLSimion-Lib/app-rlsimion.h"
#include "../../../RLSimion-Lib/features.h"
#include <iostream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace StateActionVFA
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(StateActionFunc_QArgMax)
		{
			//Because we are mostly only implementing the constructor that takes a CConfigNode,
			//it is easier to load a config file and then manipulate the classes loaded
			CConfigFile configFile;			
			CConfigNode* pConfigNode = configFile.loadFile("..\\tests\\q-learning-test.simion.exp");
			RLSimionApp *pApp = new RLSimionApp(pConfigNode);	

			CLinearStateActionVFA *pVFA= new CLinearStateActionVFA(
				pApp->pSimGod->getGlobalStateFeatureMap()
				,pApp->pSimGod->getGlobalActionFeatureMap());

			pVFA->setInitValue(0.0);
			pApp->pSimGod->deferredLoad();

			CFeatureList *pFeatures= new CFeatureList("features");

			CState* pState= pApp->pWorld->getDynamicModel()->getStateInstance();
			CAction* pAction = pApp->pWorld->getDynamicModel()->getActionInstance();
			//Q(s=2.5,a=8) = 100;
			pState->set("v-deviation", 2.5);
			pAction->set("u-thrust", 8.0);
			pVFA->getFeatures(pState, pAction, pFeatures);
			pFeatures->normalize();
			pFeatures->mult(100);
			pVFA->add(pFeatures);

			for (unsigned int i = 0; i < pFeatures->m_numFeatures; i++)
				pFeatures->m_pFeatures[i].m_factor = 1.0;
			double value = pVFA->get(pFeatures);

			//we should get the same value: 100
			if (abs(value - 100) > 0.001) Assert::Fail();

			//Q(s=2.5,a=-8) = 30
			pAction->set("u-thrust", -8.0);
			pVFA->getFeatures(pState, pAction, pFeatures);
			pFeatures->mult(30);
			pVFA->argMax(pState, pAction);
			double argMax = pAction->get(0); // we know it's only 1 action variable, so we take the shortcut
			
			if (abs(argMax - 8) > 1) Assert::Fail();

			delete pState;
			delete pAction;

			delete pFeatures;
			delete pApp;
			delete pVFA;
		}
		TEST_METHOD(StateActionFunc_FeatureMap)
		{
			//Because we are mostly only implementing the constructor that takes a CConfigNode,
			//it is easier to load a config file and then manipulate the classes loaded
			CConfigFile configFile;
			CConfigNode* pConfigNode;

			pConfigNode= configFile.loadFile("..\\tests\\q-learning-test.simion.exp");

			RLSimionApp *pApp = new RLSimionApp(pConfigNode);

			CLinearStateActionVFA *pVFA = new CLinearStateActionVFA(
				pApp->pSimGod->getGlobalStateFeatureMap()
				, pApp->pSimGod->getGlobalActionFeatureMap());

			pVFA->setInitValue(0.0);
			pApp->pSimGod->deferredLoad();

			CFeatureList *pFeatures = new CFeatureList("features");

			CState* pState = pApp->pWorld->getDynamicModel()->getStateInstance();
			CAction* pAction = pApp->pWorld->getDynamicModel()->getActionInstance();

			CState* pOutState= pApp->pWorld->getDynamicModel()->getStateInstance();
			CAction* pOutAction = pApp->pWorld->getDynamicModel()->getActionInstance();

			pState->set("v-deviation", 2.5);
			pAction->set("u-thrust", 8.0);
			pVFA->getFeatures(pState, pAction, pFeatures);
			double state = 0.0;
			double action = 0.0;

			for (unsigned int i = 0; i < pFeatures->m_numFeatures; i++)
			{
				pVFA->getFeatureStateAction(pFeatures->m_pFeatures[i].m_index
					,pOutState,pOutAction);
				state += pFeatures->m_pFeatures[i].m_factor*pOutState->get("v-deviation");
				action += pFeatures->m_pFeatures[i].m_factor*pOutAction->get("u-thrust");
			}
			if (abs(state - 2.5) > 0.1) Assert::Fail();
			if (abs(action - 8) > 0.1) Assert::Fail();


			delete pState;
			delete pAction;

			delete pFeatures;
			delete pApp;
			delete pVFA;
		}
	};
}