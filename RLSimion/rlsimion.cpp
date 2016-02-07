// RLSimion-Lib-test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "../RLSimion-Lib/world.h"
#include "../RLSimion-Lib/simgod.h"
#include "../RLSimion-Lib/reward.h"
#include "../RLSimion-Lib/states-and-actions.h"
//#include "../RLSimion-Lib/parameterscheduler.h"
#include "../RLSimion-Lib/experiment.h"




CWorld* g_pWorld;
CExperiment *g_pExperiment;

int main(int argc, char* argv[])
{
	tinyxml2::XMLDocument parameters;
	
	//CParameters *pParameters;
	if (argc > 1)
		parameters.LoadFile(argv[1]);
		//pParameters = new CParameters(argv[1]);
	else
	{
		printf("ERROR: configuration file not provided as an argument");
		exit(-1);
	}
	printf("\n\n******************\nRLSimion\n******************\nConfig. file %s\n******************\n\n", argv[1]);

	g_pWorld= new CWorld(parameters.FirstChildElement("WORLD"));
	CSimGod* pSimGod = new CSimGod(parameters.FirstChildElement("SIMGOD"));
	g_pExperiment = new CExperiment(parameters.FirstChildElement("EXPERIMENT"));
	//CParameterScheduler* pParameterScheduler= new CParameterScheduler(pParameters->getChild("PARAMETER_SCHEDULER"));

	CState *s= g_pWorld->getStateInstance();
	CState *s_p= g_pWorld->getStateInstance();
	CAction *a= g_pWorld->getActionInstance();

	double r= 0.0;
	double td= 0.0;

	//episodes
	for (g_pExperiment->m_expProgress.setEpisode(1); g_pExperiment->m_expProgress.isValidEpisode(); g_pExperiment->m_expProgress.incEpisode())
	{
		g_pWorld->reset(s);

		//steps per episode
		for (g_pExperiment->m_expProgress.setStep(1); g_pExperiment->m_expProgress.isValidStep(); g_pExperiment->m_expProgress.incStep())
		{

			//Update parameters that have a schedule: learning gains, exploration, ...
		//	pParameterScheduler->update();

			//a= pi(s)
			pSimGod->selectAction(s,a);

			//s_p= f(s,a); r= R(s');
			r= g_pWorld->executeAction(s,a,s_p);

			//update god's policy and value estimation
			pSimGod->update(s, a, s_p, r);

			//log tuple <s,a,s',r>
			g_pExperiment->logStep(s,a,s_p,g_pWorld->getReward()); //we need the complete reward vector for logging

			//s= s'
			s->copy(s_p);
		}
	}

	delete pSimGod;


	delete s;
	delete s_p;
	delete a;

	//delete pParameterScheduler;
	delete pParameters;
	delete g_pWorld;
	delete g_pExperiment;

	return 0;
}