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

#include "reward.h"
#include "config.h"
#include "worlds/world.h"
#include "parameters-numeric.h"
#include "app.h"
#include "logger.h"
#include "../Common/named-var-set.h"

#include <math.h>
#include <algorithm>

ToleranceRegionReward::ToleranceRegionReward(string variable, double tolerance, double scale)
{
	m_name= "r/(" + variable + ")";
	m_pVariableName = variable.c_str();
	m_tolerance = tolerance;
	m_scale = scale;
}

/// <summary>
/// This reward function returns a reward depending on the value of an error variable with respect to its tolerance region
/// </summary>
/// <param name="s">Initial state</param>
/// <param name="a">Action</param>
/// <param name="s_p">Resultant state</param>
/// <returns></returns>
double ToleranceRegionReward::getReward(const State *s, const Action* a, const State *s_p)
{
	double rew, error;

	error = s_p->get(m_pVariableName.c_str());

	error = (error) / m_tolerance;

	rew = m_maxReward - fabs(error);
	
	rew = std::max(m_minReward, rew);

	return rew;
}

const char* ToleranceRegionReward::getName(){ return m_name.c_str(); }


RewardFunction::RewardFunction()
{
	m_bInitialized = false;
}

RewardFunction::~RewardFunction()
{
	//we have to delete the reward components
	for (auto it = m_rewardComponents.begin(); it != m_rewardComponents.end(); ++it) delete *it;
}

/// <summary>
/// RewardFunction can use more than one scalar reward and they are added using this method. Scalar rewards
/// must derive from IRewardComponent
/// </summary>
/// <param name="rewardComponent">The new scalar reward to be added</param>
void RewardFunction::addRewardComponent(IRewardComponent* rewardComponent)
{
	m_rewardComponents.push_back(rewardComponent);
}

/// <summary>
/// Calculates the total reward based on the different scalar rewards. If we only define one reward function, its value
/// will be returned
/// </summary>
/// <param name="s">Initial state</param>
/// <param name="a">Action</param>
/// <param name="s_p">Resultant state</param>
/// <returns>The (scalarized) final reward calculated from all the different reward signals</returns>
double RewardFunction::getReward(const State* s, const Action* a, const State* s_p)
{
	if (!m_bInitialized)
		SimionApp::get()->pLogger->logMessage(MessageType::Error, "Reward has not been initialized, can't use it");

	if (m_bOverride)
	{
		m_bOverride = false;
		return m_overrideValue;
	}

	double reward = 0.0, r_i;
	int i = 0;
	for (auto it = m_rewardComponents.begin(); it != m_rewardComponents.end(); ++it)
	{
		r_i = (*it)->getReward(s, a, s_p);
		m_pRewardVector->set(i, r_i);
		reward += r_i;
		++i;
	}
	return reward;
}

/// <summary>
/// DynamicModel subclasses should call this initialization method after adding the reward functions
/// </summary>
void RewardFunction::initialize()
{
	size_t numComponents;
	if (!m_bInitialized)
	{
		//create the reward vector and set names
		numComponents = m_rewardComponents.size();
		
		for (size_t i = 0; i < numComponents; ++i)
		{
			rewardDescriptor.addVariable(m_rewardComponents[i]->getName(), "unitless"
				, m_rewardComponents[i]->getMin(), m_rewardComponents[i]->getMax());
		}
		m_pRewardVector = rewardDescriptor.getInstance();
		for (unsigned int i= 0; i<numComponents; ++i)
		{
			m_pRewardVector->getProperties(i)->setName(m_rewardComponents[i]->getName());
		}
		
		m_bInitialized = true;
	}
	else SimionApp::get()->pLogger->logMessage(MessageType::Warning, "Reward function already initialized. Can't initialize again");
}


Reward* RewardFunction::getRewardVector()
{
	if (!m_bInitialized)
		SimionApp::get()->pLogger->logMessage(MessageType::Error, "Reward has not been initialized, can't use it");

	return m_pRewardVector;
}


/// <summary>
/// If we want to override the final reward in some special states (i.e. a negative reward if FAST simulator crashed)
/// we can call this method from the DynamicModel
/// </summary>
/// <param name="reward">The reward we want to give the agent</param>
void RewardFunction::override(double reward)
{
	m_bOverride = true;
	m_overrideValue = reward;
}