#include "stdafx.h"
#include "reward.h"
#include "named-var-set.h"
#include "parameterized-object.h"
#include "parameters.h"
#include "world.h"
#include "globals.h"
#include "parameters-numeric.h"
#include "app.h"
#include "logger.h"

CToleranceRegionReward::CToleranceRegionReward(char* variable, double tolerance, double scale)
{
	sprintf_s(m_name, MAX_REWARD_NAME_SIZE, "r/%s)", variable);
	m_pVariableName = variable;
	m_tolerance = tolerance;
	m_scale = scale;
}
double CToleranceRegionReward::getReward(const CState *s, const CAction* a, const CState *s_p)
{
	double rew, error;

	error = s_p->getValue(m_pVariableName);

	error = (error) / m_tolerance;

	rew = m_maxReward - fabs(error);
	
	rew = std::max(m_minReward, rew);

	return rew;
}

const char* CToleranceRegionReward::getName(){ return m_name; }


CRewardFunction::CRewardFunction()
{
	m_bInitialized = false;
}

CRewardFunction::~CRewardFunction()
{
	//we have to delete the reward components
	for (auto it = m_rewardComponents.begin(); it != m_rewardComponents.end(); ++it) delete *it;
}

void CRewardFunction::addRewardComponent(IRewardComponent* rewardComponent)
{
	m_rewardComponents.push_back(rewardComponent);
}

double CRewardFunction::getReward(const CState* s, const CAction* a, const CState* s_p)
{
	if (!m_bInitialized)
		CApp::get()->pLogger->logMessage(MessageType::Error, "Reward has not been initialized, can't use it");

	double reward = 0.0, r_i;
	int i = 0;
	for (auto it = m_rewardComponents.begin(); it != m_rewardComponents.end(); ++it)
	{
		r_i = (*it)->getReward(s, a, s_p);
		m_pRewardVector->setValue(i, r_i);
		reward += r_i;
		++i;
	}
	return reward;
}

void CRewardFunction::initialize()
{
	int numComponents;
	if (!m_bInitialized)
	{
		//create the reward vector and set names
		numComponents = m_rewardComponents.size();
		m_pRewardVector = new CReward(numComponents);
		for (int i = 0; i < numComponents; ++i)
		{
			m_pRewardVector->setName(i, m_rewardComponents[i]->getName());
		}
		m_bInitialized = true;
	}
	else CApp::get()->pLogger->logMessage(MessageType::Warning, "Reward function already initialized. Can't initialize again");
}


CReward* CRewardFunction::getRewardVector()
{
	if (!m_bInitialized)
		CApp::get()->pLogger->logMessage(MessageType::Error, "Reward has not been initialized, can't use it");

	return m_pRewardVector;
}