#include "stdafx.h"
#include "simion.h"

#include "config.h"
#include "controller.h"
#include "actor-critic.h"
#include "q-learners.h"

std::shared_ptr<CSimion> CSimion::getInstance(CConfigNode* pConfigNode)
{
	return CHOICE("Type", "The Simion class",
	{
	CHOICE_ELEMENT_FACTORY(pConfigNode,"Controller", CController, "The controller",""),
	CHOICE_ELEMENT_NEW(pConfigNode,CActorCritic, "Actor-Critic", "Actor-Critic agent"),
	CHOICE_ELEMENT_NEW(pConfigNode,CQLearning, "Q-Learning", "Q-Learning agent"),
	CHOICE_ELEMENT_NEW(pConfigNode,CDoubleQLearning, "Double-Q-Learning", "Double-Q-Learning agent"),
	CHOICE_ELEMENT_NEW(pConfigNode,CSARSA, "SARSA", "A SARSA agent"),
	CHOICE_ELEMENT_NEW(pConfigNode,CIncrementalNaturalActorCritic, "Inc-Natural-Actor-Critic", "Incremental-Natural-Actor-Critic as in 'Model-free Reinforcement Learning with Continuous Action in Practice'")
	});
	END_CHOICE();
	return 0;
	//CHOICE("Type","The Simion class");
	//CHOICE_ELEMENT_FACTORY("Controller", CController,"Controller");
	//CHOICE_ELEMENT("Actor-Critic", CActorCritic,"Actor-Critic agent");
	//CHOICE_ELEMENT("Q-Learning", CQLearning, "Q-Learning agent");
	//CHOICE_ELEMENT("Double-Q-Learning", CDoubleQLearning, "Double-Q-Learning agent");
	//CHOICE_ELEMENT("SARSA", CSARSA, "A SARSA agent");
	//CHOICE_ELEMENT("Inc-Natural-Actor-Critic", CIncrementalNaturalActorCritic, "Incremental-Natural-Actor-Critic as in 'Model-free Reinforcement Learning with Continuous Action in Practice'");
	//END_CHOICE();
	//return 0;
	END_CLASS();
}