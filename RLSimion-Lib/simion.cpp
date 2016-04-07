#include "stdafx.h"
#include "simion.h"
#include "globals.h"
#include "parameters.h"
#include "controller.h"
#include "actor-critic.h"
#include "q-learners.h"

CLASS_FACTORY(CSimion)
{
	CHOICE("Type","The Simion class");
	CHOICE_ELEMENT_FACTORY("Controller", CController,"A controller");
	CHOICE_ELEMENT("Actor-Critic", CActorCritic,"An Actor-Critic agent");
	CHOICE_ELEMENT("Q-Learning", CQLearning, "A Q-Learning agent");
	CHOICE_ELEMENT("SARSA", CSARSA, "A SARSA agent");
	CHOICE_ELEMENT("Inc-Natural-Actor-Critic", CIncrementalNaturalActorCritic, "Incremental-Natural-Actor-Critic as in 'Model-free Reinforcement Learning with Continuous Action in Practice'");
	END_CHOICE();
	return 0;
	END_CLASS();
}