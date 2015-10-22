// RLSimion-Lib-test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../RLSimion-Lib/parameters.h"
#include "../RLSimion-Lib/world.h"
#include "../RLSimion-Lib/simgod.h"
#include "../RLSimion-Lib/reward.h"
#include "../RLSimion-Lib/states-and-actions.h"
#include "../RLSimion-Lib/parameterscheduler.h"
#include "../RLSimion-Lib/experiment.h"

#ifdef _DEBUG
	#pragma comment (lib,"../Debug/RLSimion-Lib.lib")
#else
	#pragma comment (lib,"../Release/RLSimion-Lib.lib")
#endif

#include <stdlib.h>
#include <stdio.h>

CParameters *g_pParameters;
CWorld *g_pWorld;
CSimGod *g_pSimGod;
CReward *g_pReward;
CParameterScheduler *g_pParameterScheduler;
CExperiment *g_pExperiment;

int main(int argc, char* argv[])
{
	//Global parameters and objects
	g_pParameters= new CParameters("../config/underwater-vehicle/learn.txt");

	g_pWorld= new CWorld(g_pParameters->getStringPtr("WORLD_CONFIG_FILE"));

	g_pSimGod = new CSimGod(g_pParameters);

	
	g_pParameterScheduler= new CParameterScheduler(g_pParameters->getStringPtr("PARAMETER_SCHEDULER_CONFIG_FILE"));
	g_pExperiment= new CExperiment(g_pParameters);

	CState *s= g_pWorld->getStateInstance();
	CState *s_p= g_pWorld->getStateInstance();
	CAction *a= g_pWorld->getActionInstance();

	double r= 0.0;
	double td= 0.0;
	double rho= 0.0; //sample importance weight


	//episodes
	for (g_pExperiment->m_episode= 1; g_pExperiment->m_episode<=g_pExperiment->m_numEpisodes; g_pExperiment->m_episode++)
	{
		g_pWorld->reset(s);

		//steps per episode
		for (g_pExperiment->m_step= 1; g_pExperiment->m_step<=g_pExperiment->m_numSteps; g_pExperiment->m_step++)
		{

			//Update parameters that have a schedule: learning gains, exploration, ...
			if (g_pParameterScheduler)
				g_pParameterScheduler->update();

			//a= pi(s)
			g_pSimGod->selectAction(s,a);

			//s_p= f(s,a); r= R(s');
			r= g_pWorld->executeAction(s,a,s_p);

			//update god's policy and value estimation
			g_pSimGod->update(s, a, s_p, r);


			//log tuple <s,a,s',r>
			g_pExperiment->logStep(s,a,s_p,r);

			//s= s'
			s->copy(s_p);
		}
	}

	delete g_pSimGod;

	printf("\nThe END\n");
	char c;
	scanf_s("%c",&c);

	delete s;
	delete s_p;
	delete a;

	delete g_pParameters;
	delete g_pWorld;
	
	delete g_pParameterScheduler;
	delete g_pExperiment;

	return 0;
}
//
//
//
//#include "stdafx.h"
//#include "EnvironmentManager.h"
//#include "RewardManager.h"
//#include "StateManager.h"
//#include "Learner.h"
//#include "Actions.h"
//#include "cacrobotmodel.h"
//#include "FileManager.h"
//
//CParameters *g_pParameters;
///*
//#define NUM_PARTITIONS_X 30
//#define NUM_PARTITIONS_Y 30*/
//
//
//
//
//
//int _tmain(int argc, _TCHAR* argv[])
//{
////	DebugInit("debug.txt","+",false); //debug levels!!
//
//	SetWorkPath();
//
//	//PARAMETER FILENAME
//	char parameterFileName[256];
//
//	if (argc>1) sprintf_s(parameterFileName,256,"%S",argv[1]);
//	else
//	{
//		printf("ERROR: parameter file is missing. Using default file experiments/config/parameters.txt\n");
//		strcpy_s(parameterFileName,256,"parameters.txt");
//	}
//	CLearnerManager LearnerManager;
//
//	g_pParameters= new CParameters(parameterFileName);
//	
//
//	CEnvironmentManager envManager;
//
//	CContinuousTimeAndActionTransitionFunction *pTransitionFunction=
//		envManager.getModel(g_pParameters->getStringParameter("ENVIRONMENT"));
//	CTransitionFunctionEnvironment* pModel= new CTransitionFunctionEnvironment(pTransitionFunction);
//
//	CStateReward *pReward;
//	pReward = new CEnvironmentReward(pTransitionFunction);
//
//	CAgent *pAgent= new CAgent(pModel);
//
//	
//
//	//ACTIONS
//	AddActionsToAgent(pAgent,pTransitionFunction);
//
//	pAgent->addSemiMDPListener(new CAverageRewardCalculator(pAgent,pReward,1,1,1));
//
//	
//	LearnerManager.init(pAgent,pModel,pReward
//		,g_pParameters->getStringParameter("ACTOR")
//		,g_pParameters->getStringParameter("CRITIC")
//		,g_pParameters->getStringParameter("CONTROLLER"));
//
//
//
//	int nStep;
//	CState *pState= new CState(pTransitionFunction->getStateProperties());
//
//
//	printf("%s\n",LearnerManager.getExperimentalSetupName());
//	
//
//
//	//SAVE PARAMETERS
//	/*char parametersFilename[256];
//	sprintf_s(parametersFilename,256,"%s/parameters.txt",dirname);
//	LearnerManager.saveParameters(pAgent,parametersFilename);*/
//
//	string outputDir= g_pParameters->getStringParameter("OUTPUT_DIR");
//	string logFilePrefix= g_pParameters->getStringParameter("LOG_FILE_PREFIX");
//
//	double randomSeed= g_pParameters->getParameter("RANDOM_SEED");
//
//	printf("OUTPUT_DIR=%s\n",outputDir.c_str());
//	printf("RANDOM_SEED=%d\n",(int)randomSeed);
//
//
//	LearnerManager.LoadData();
//
//	CLogFile logFile(pTransitionFunction,pReward);
//
//	CSimpleLogFile avgRewardLogFile;
//	char pAvgRewardLogFilename[512];
//	sprintf_s(pAvgRewardLogFilename,256,"%s-avg-reward-%d.txt",logFilePrefix.c_str(),(int)randomSeed);
//	avgRewardLogFile.Init(pAvgRewardLogFilename);
//
//
//	for (int i= 1; i<=g_pParameters->getParameter("NUM_EPISODES"); i++)
//	{
//		bool bLogEpisode;
//		bool bEvaluation;
//
//		char logFilename[256];
//
//		if (g_pParameters->getParameter("EVALUATE")!=0.0 
//			&& (i-1)%(int)g_pParameters->getParameter("EVALUATION_FREQ")==0)
//			
//		{
//			bLogEpisode= true; bEvaluation= true;
//			LearnerManager.setExplorationParameter(pAgent,0.0);
//		}
//		else
//		{
//			bLogEpisode= g_pParameters->getParameter("LOG_TRAINING_EPISODES")!=0.0;
//			bEvaluation= false;
//			LearnerManager.setExplorationParameter(pAgent,1.-((double)i/g_pParameters->getParameter("NUM_EPISODES")));
//		}
//
//		sprintf_s(logFilename,256,"%s-log-%d-%d.txt",logFilePrefix.c_str(),(int)randomSeed,i);
//		if (bLogEpisode)
//			logFile.Init(logFilename);
//
//
//		double rewards= 0;
//		double avgReward= 0.0;
//
//		nStep= 0;
//		pAgent->startNewEpisode();
//		do
//		{
//			pAgent->doControllerStep();
//			pModel->getState(pState);
//			if (bLogEpisode)
//				logFile.AddLogLine(pState,pAgent->getLastAction(),nStep);
//			
//			rewards+= ((CEnvironmentReward*)pReward)->getLastReward();
//			nStep++;
//
//			printf ("%d/%d episodes step %d (reward=%.5f)\r",
//				i,(int)g_pParameters->getParameter("NUM_EPISODES"),nStep,rewards/(double)nStep);
//		}
//		while (nStep<(int)g_pParameters->getParameter("TIME")/g_pParameters->getParameter("DELTA_T"));
//
//
//		//global stats
//		avgReward= rewards/((int)g_pParameters->getParameter("TIME")/g_pParameters->getParameter("DELTA_T"));
//		if (bEvaluation) avgRewardLogFile.AddLogLine(i,avgReward);
//
//		//EXPORT MATRICES
//		LearnerManager.ExportData(i);
//	}
//
//
//	
//	delete pState;
//	delete g_pParameters;
//	delete pAgent;
//
//
//	return 0;
//}
//
