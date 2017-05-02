#include "stdafx.h"
#include "simion.h"

#include "config.h"
#include "controller.h"
#include "actor-critic.h"
#include "q-learners.h"

std::shared_ptr<CSimion> CSimion::getInstance(CConfigNode* pConfigNode)
{
	return CHOICE<CSimion>(pConfigNode,"Type", "The Simion class",
	{
		{"Controller",CHOICE_ELEMENT_FACTORY<CController>},
		{"Actor-Critic",CHOICE_ELEMENT_NEW<CActorCritic>},
		{"Q-Learning",CHOICE_ELEMENT_NEW<CQLearning>},
		{"Double-Q-Learning",CHOICE_ELEMENT_NEW<CDoubleQLearning>},
		{"SARSA",CHOICE_ELEMENT_NEW<CSARSA>},
		{"Inc-Natural-Actor-Critic",CHOICE_ELEMENT_NEW<CIncrementalNaturalActorCritic>}
	});
}

