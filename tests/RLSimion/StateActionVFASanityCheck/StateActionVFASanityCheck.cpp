// StateActionVFASanityCheck.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../../../RLSimion-Lib/vfa.h"
#include "../../../RLSimion-Lib/app-rlsimion.h"
#include "../../../RLSimion-Lib/features.h"
#include <iostream>
#pragma comment (lib,"../../../Debug/RLSimion-Lib.lib")
#pragma comment (lib,"../../../Debug/tinyxml2.lib")
#pragma comment (lib,"../../../Debug/WindowsUtils.lib")

int main()
{
	//m_sVSetpoint = addStateVariable("v-setpoint", "m/s", -5.0, 5.0);
	//m_sV = addStateVariable("v", "m/s", -5.0, 5.0);
	//m_sVDeviation = addStateVariable("v-deviation", "m/s", -10.0, 10.0);

	//m_aUThrust = addActionVariable("u-thrust", "N", -30.0, 30.0);


	//it is easier to load a config file and then manipulate the classes loaded
	CConfigFile configFile;
	CConfigNode* pConfigNode = configFile.loadFile("../tests/q-learning-test.exp");
	RLSimionApp *pApp = new RLSimionApp(pConfigNode);

	CLinearStateActionVFA *pVFA = new CLinearStateActionVFA(
		pApp->pSimGod->getGlobalStateFeatureMap()
		, pApp->pSimGod->getGlobalActionFeatureMap());

	pVFA->setInitValue(0.0);
	pVFA->deferredLoadStep();

	CFeatureList *pFeatures = new CFeatureList();

	//CState* pState = pApp->pWorld->getDynamicModel()->getStateInstance();
	//CAction* pAction = pApp->pWorld->getDynamicModel()->getActionInstance();
	////Q(s=2.5,a=8) = 100;
	//pState->set("v-deviation", 2.5);
	//pAction->set("u-thrust", 8.0);
	//pVFA->getFeatures(pState, pAction, pFeatures);
	//pFeatures->normalize();
	//pFeatures->mult(100);
	//pVFA->add(pFeatures); //we add the features (sum=100) to the weights


	//for (int i = 0; i < pFeatures->m_numFeatures; i++)
	//	pFeatures->m_pFeatures[i].m_factor = 1.0;
	//double value = pVFA->get(pFeatures);

	////Q(s=2.5,a=-8) = 30
	//pAction->set("u-thrust", -8.0);
	//pVFA->getFeatures(pState, pAction, pFeatures);
	//pFeatures->mult(30);
	//pVFA->argMax(pState, pAction);
	//double argMax = pAction->get(0); // we know it's only 1 action variable, so we take the shortcut

	CState* pState = pApp->pWorld->getDynamicModel()->getStateInstance();
	CAction* pAction = pApp->pWorld->getDynamicModel()->getActionInstance();

	CState* pOutState = pApp->pWorld->getDynamicModel()->getStateInstance();
	CAction* pOutAction = pApp->pWorld->getDynamicModel()->getActionInstance();

	pState->set("v-deviation", 2.5);
	pAction->set("u-thrust", 8.0);
	pVFA->getFeatures(pState, pAction, pFeatures);
	double state = 0.0;
	double action = 0.0;

	for (int i = 0; i < pFeatures->m_numFeatures; i++)
	{
		pVFA->getFeatureStateAction(pFeatures->m_pFeatures[i].m_index
			, pOutState, pOutAction);
		state += pFeatures->m_pFeatures[i].m_factor*pOutState->get("v-deviation");
		action += pFeatures->m_pFeatures[i].m_factor*pOutAction->get("u-thrust");
	}


	return 0;
}

