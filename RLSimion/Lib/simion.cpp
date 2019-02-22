/*
	SimionZoo: A framework for online model-free Reinforcement Learning on continuous
	control problems

	Copyright (c) 2016 SimionSoft. https://github.com/simionsoft

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#include "simion.h"

#include "config.h"
#include "controller.h"
#include "actor-critic.h"
#include "q-learners.h"
#include "DQN.h"
#include "DDPG.h"
//#include "async-deep-simion.h"

std::shared_ptr<Simion> Simion::getInstance(ConfigNode* pConfigNode)
{

	return CHOICE<Simion>(pConfigNode, "Type", "The Simion class",
	{
		{"Controller", CHOICE_ELEMENT_FACTORY<Controller>},
		{"Actor-Critic", CHOICE_ELEMENT_NEW<ActorCritic>},
		{"Q-Learning", CHOICE_ELEMENT_NEW<QLearning>},
		{"Double-Q-Learning", CHOICE_ELEMENT_NEW<DoubleQLearning>},
		{"SARSA", CHOICE_ELEMENT_NEW<SARSA>},
		{"Inc-Natural-Actor-Critic", CHOICE_ELEMENT_NEW<IncrementalNaturalActorCritic>},
		{"Off-Policy-Actor-Critic", CHOICE_ELEMENT_NEW<OffPolicyActorCritic>}
#if defined(__linux__) || defined(_WIN64)
		,{"DQN", CHOICE_ELEMENT_NEW<DQN>}
		,{"Double-DQN", CHOICE_ELEMENT_NEW<DoubleDQN>}
		,{ "DDPG", CHOICE_ELEMENT_NEW<DDPG> }
#endif
		/*,{ "Async-QL-earning", CHOICE_ELEMENT_NEW<AsyncQLearning > }*/
	});
}

