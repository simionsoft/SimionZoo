// RLSimion-Lib-test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "../RLSimion-Lib/world.h"
#include "../RLSimion-Lib/simgod.h"
#include "../RLSimion-Lib/reward.h"
#include "../RLSimion-Lib/named-var-set.h"
#include "../RLSimion-Lib/globals.h"
#include "../RLSimion-Lib/experiment.h"
#include "../RLSimion-Lib/parameters.h"
#include "../RLSimion-Lib/logger.h"

int main(int argc, char* argv[])
{
	CParameterFile xmlDoc;
	CParameters* pParameters;

	if (argc > 1)
	{
		pParameters = xmlDoc.loadFile(argv[1], "RLSimion");
		if (!pParameters)
		{
			printf("Error loading configuration file: %s\n\n", xmlDoc.getError());
			getchar();
			exit(-1);
		}
	}
	else
	{
		printf("ERROR: configuration file not provided as an argument");
		exit(-1);
	}
	printf("\n\n******************\nRLSimion\n******************\nConfig. file %s\n******************\n\n", argv[1]);

	RLSimion::init(pParameters);

	//create state and action vectors
	CState *s = CWorld::getDynamicModel()->getStateDescriptor()->getInstance();
	CState *s_p = CWorld::getDynamicModel()->getStateDescriptor()->getInstance();
	CAction *a = CWorld::getDynamicModel()->getActionDescriptor()->getInstance();
	//register the state and action vectors in the logger
	RLSimion::g_pLogger->addVarSetToStats("State", s);
	RLSimion::g_pLogger->addVarSetToStats("Action", a);
	RLSimion::g_pLogger->addVarToStats("Reward", "sum", RLSimion::g_pWorld->getScalarReward());
	RLSimion::g_pLogger->addVarSetToStats("Reward", RLSimion::g_pWorld->getReward());

	double r= 0.0;

	//episodes
	for (RLSimion::g_pExperiment->nextEpisode(); RLSimion::g_pExperiment->isValidEpisode(); RLSimion::g_pExperiment->nextEpisode())
	{
		RLSimion::g_pWorld->reset(s);

		//steps per episode
		for (RLSimion::g_pExperiment->nextStep(); RLSimion::g_pExperiment->isValidStep(); RLSimion::g_pExperiment->nextStep())
		{
			//a= pi(s)
			RLSimion::g_pSimGod->selectAction(s,a);

			//s_p= f(s,a); r= R(s');
			r= RLSimion::g_pWorld->executeAction(s,a,s_p);

			//update god's policy and value estimation
			RLSimion::g_pSimGod->update(s, a, s_p, r);

			//log tuple <s,a,s',r>
			RLSimion::g_pExperiment->timestep(s,a,s_p,RLSimion::g_pWorld->getReward()); //we need the complete reward vector for logging

			//s= s'
			s->copy(s_p);
		}
	}

	//pending task: rewrite using smart pointers
	RLSimion::shutdown();

	delete s;
	delete s_p;
	delete a;

	return 0;
}