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

	CParameters* pParameters= 0;

	try
	{

		RLSimion::init(argc, argv, "RLSimion");

		//create state and action vectors
		CState *s = CWorld::getDynamicModel()->getStateDescriptor()->getInstance();
		CState *s_p = CWorld::getDynamicModel()->getStateDescriptor()->getInstance();
		CAction *a = CWorld::getDynamicModel()->getActionDescriptor()->getInstance();
		//register the state and action vectors in the logger
		RLSimion::Logger.addVarSetToStats("State", s);
		RLSimion::Logger.addVarSetToStats("Action", a);
		RLSimion::Logger.addVarToStats("Reward", "sum", RLSimion::World.getScalarReward());
		RLSimion::Logger.addVarSetToStats("Reward", RLSimion::World.getReward());

		double r = 0.0;

		//episodes
		for (RLSimion::Experiment.nextEpisode(); RLSimion::Experiment.isValidEpisode(); RLSimion::Experiment.nextEpisode())
		{
			RLSimion::World.reset(s);

			//steps per episode
			for (RLSimion::Experiment.nextStep(); RLSimion::Experiment.isValidStep(); RLSimion::Experiment.nextStep())
			{
				//a= pi(s)
				RLSimion::SimGod.selectAction(s, a);

				//s_p= f(s,a); r= R(s');
				r = RLSimion::World.executeAction(s, a, s_p);

				//update god's policy and value estimation
				RLSimion::SimGod.update(s, a, s_p, r);

				//log tuple <s,a,s',r>
				RLSimion::Experiment.timestep(s, a, s_p, RLSimion::World.getReward()); //we need the complete reward vector for logging

				//s= s'
				s->copy(s_p);
			}
		}

		//pending task: rewrite using smart pointers
		RLSimion::shutdown();

		delete s;
		delete s_p;
		delete a;
	}
	catch (std::exception& e)
	{
		CLogger::logMessage(MessageType::Error, e.what());
	}

	return 0;
}