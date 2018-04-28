#include "stdafx.h"
#include "CppUnitTest.h"
#include "../../../RLSimion/vfa.h"
#include "../../../RLSimion/app-rlsimion.h"
#include "../../../RLSimion/features.h"
#include "../../../RLSimion/simgod.h"
#include "../../../RLSimion/worlds/world.h"
#include "../../../RLSimion/named-var-set.h"
#include <iostream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace StateActionVFA
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(QFunction_ArgMax)
		{
			//Because we are mostly only implementing the constructor that takes a ConfigNode,
			//it is easier to load a config file and then manipulate the classes loaded
			ConfigFile configFile;			
			ConfigNode* pConfigNode = configFile.loadFile("..\\tests\\q-learning-test.simion.exp");
			RLSimionApp *pApp = new RLSimionApp(pConfigNode);	

			LinearStateActionVFA *pVFA= new LinearStateActionVFA(
				pApp->pSimGod->getGlobalStateFeatureMap()
				,pApp->pSimGod->getGlobalActionFeatureMap());

			pVFA->setInitValue(0.0);
			pApp->pSimGod->deferredLoad();

			FeatureList *pFeatures= new FeatureList("features");

			State* pState= pApp->pWorld->getDynamicModel()->getStateInstance();
			Action* pAction = pApp->pWorld->getDynamicModel()->getActionInstance();
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
		TEST_METHOD(QFunction_FeatureMap)
		{
			//Because we are mostly only implementing the constructor that takes a ConfigNode,
			//it is easier to load a config file and then manipulate the classes loaded
			ConfigFile configFile;
			ConfigNode* pConfigNode;

			pConfigNode= configFile.loadFile("..\\tests\\q-learning-test.simion.exp");

			RLSimionApp *pApp = new RLSimionApp(pConfigNode);

			LinearStateActionVFA *pVFA = new LinearStateActionVFA(
				pApp->pSimGod->getGlobalStateFeatureMap()
				, pApp->pSimGod->getGlobalActionFeatureMap());

			pVFA->setInitValue(0.0);
			pApp->pSimGod->deferredLoad();

			FeatureList *pFeatures = new FeatureList("features");

			State* pState = pApp->pWorld->getDynamicModel()->getStateInstance();
			Action* pAction = pApp->pWorld->getDynamicModel()->getActionInstance();

			State* pOutState= pApp->pWorld->getDynamicModel()->getStateInstance();
			Action* pOutAction = pApp->pWorld->getDynamicModel()->getActionInstance();

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

		TEST_METHOD(QFunction_ArgMax_two_actions)
		{
			//Because we are mostly only implementing the constructor that takes a ConfigNode,
			//it is easier to load a config file and then manipulate the classes loaded
			ConfigFile configFile;
			ConfigNode* pConfigNode = configFile.loadFile("..\\tests\\robot-control-test.simion.exp");
			RLSimionApp *pApp = new RLSimionApp(pConfigNode);

			LinearStateActionVFA *pVFA = new LinearStateActionVFA(
				pApp->pSimGod->getGlobalStateFeatureMap()
				, pApp->pSimGod->getGlobalActionFeatureMap());

			pVFA->setInitValue(0.0);
			pApp->pSimGod->deferredLoad();

			FeatureList *pFeatures = new FeatureList("features");

			State* pState = pApp->pWorld->getDynamicModel()->getStateInstance();
			Action* pAction = pApp->pWorld->getDynamicModel()->getActionInstance();
			//Q(s={0.0,1.0,0.0},a={1.0,1.0})= 100;
			pState->set("robot1-x", 0.0);
			pState->set("robot1-y", 1.0);
			pState->set("robot1-theta", 0.0);
			pAction->set("robot1-v", 1.0);
			pAction->set("robot1-omega", 0.5);

			pVFA->getFeatures(pState, pAction, pFeatures);
			pFeatures->normalize();
			pFeatures->mult(100);
			pVFA->add(pFeatures);

			for (unsigned int i = 0; i < pFeatures->m_numFeatures; i++)
				pFeatures->m_pFeatures[i].m_factor = 1.0;
			double value = pVFA->get(pFeatures);

			//we should get the same value: 100
			if (abs(value - 100) > 0.001) Assert::Fail();

			//Q(s={0.0,1.0,0.0},a={-1.0,0.0})= 10;
			pState->set("robot1-x", 0.0);
			pState->set("robot1-y", 1.0);
			pState->set("robot1-theta", 0.0);
			pAction->set("robot1-v", -1.0);
			pAction->set("robot1-omega", -0.5);

			pVFA->getFeatures(pState, pAction, pFeatures);
			pFeatures->mult(10);
			pVFA->argMax(pState, pAction);
			double argMaxV = pAction->get("robot1-v");
			if (abs(argMaxV - 1.0) > 0.1) Assert::Fail();
			
			double argMaxOmega = pAction->get("robot1-omega");
			if (abs(argMaxOmega - 0.5) > 0.1) Assert::Fail();

			delete pState;
			delete pAction;

			delete pFeatures;
			delete pApp;
			delete pVFA;
		}
	};
}