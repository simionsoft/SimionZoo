#include "reward.h"
#include "config.h"
#include "worlds/world.h"
#include "parameters-numeric.h"
#include "app.h"
#include "logger.h"
#include "named-var-set.h"
#include <algorithm>

CToleranceRegionReward::CToleranceRegionReward(string variable, double tolerance, double scale)
{
	m_name= "r/(" + variable + ")";
	m_pVariableName = variable.c_str();
	m_tolerance = tolerance;
	m_scale = scale;
}
double CToleranceRegionReward::getReward(const CState *s, const CAction* a, const CState *s_p)
{
	double rew, error;

	error = s_p->get(m_pVariableName.c_str());

	error = (error) / m_tolerance;

	rew = m_maxReward - fabs(error);
	
	rew = std::max(m_minReward, rew);

	return rew;
}

const char* CToleranceRegionReward::getName(){ return m_name.c_str(); }


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
		CSimionApp::get()->pLogger->logMessage(MessageType::Error, "Reward has not been initialized, can't use it");

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

void CRewardFunction::initialize()
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
			m_pRewardVector->getProperties(i).setName(m_rewardComponents[i]->getName());
		}
		
		m_bInitialized = true;
	}
	else CSimionApp::get()->pLogger->logMessage(MessageType::Warning, "Reward function already initialized. Can't initialize again");
}


CReward* CRewardFunction::getRewardVector()
{
	if (!m_bInitialized)
		CSimionApp::get()->pLogger->logMessage(MessageType::Error, "Reward has not been initialized, can't use it");

	return m_pRewardVector;
}

void CRewardFunction::override(double reward)
{
	m_bOverride = true;
	m_overrideValue = reward;
}