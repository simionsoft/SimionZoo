#include "featuremap.h"
#include "named-var-set.h"
#include "worlds/world.h"
#include "features.h"
#include "config.h"
#include "single-dimension-grid.h"

BagStateFeatureMap::BagStateFeatureMap(ConfigNode* pParameters) : StateFeatureMap(pParameters)
{
	m_stateVariables = MULTI_VALUE_VARIABLE<STATE_VARIABLE>(pParameters, "State-Variables", "The state variables");

	for (unsigned int i = 0; i< m_stateVariables.size(); i++)
	{
		m_inputStateVariables.push_back(m_stateVariables[i]->getName());
	}
}

BagActionFeatureMap::BagActionFeatureMap(ConfigNode* pParameters) : ActionFeatureMap(pParameters)
{
	m_actionVariables = MULTI_VALUE_VARIABLE<ACTION_VARIABLE>(pParameters, "Action-Variables", "The action variables");

	for (unsigned int i = 0; i< m_actionVariables.size(); i++)
	{
		m_inputActionVariables.push_back(m_actionVariables[i]->getName());
	}
}

void BagStateFeatureMap::getFeatures(const State* s, FeatureList* outFeatures)
{
	outFeatures->clear();
	for (size_t i = 0; i < m_stateVariables.size(); i++)
	{
		int index = m_stateVariables[i]->get();
		outFeatures->add(index, s->get(index));
	}
}

void BagStateFeatureMap::getFeatureState(unsigned int feature, State* s)
{

}

void BagActionFeatureMap::getFeatures(const Action* a, FeatureList* outFeatures)
{
	outFeatures->clear();
	for (size_t i = 0; i < m_actionVariables.size(); i++)
	{
		int index = m_actionVariables[i]->get();
		outFeatures->add(index, a->get(index));
	}
}

void BagActionFeatureMap::getFeatureAction(unsigned int feature, Action* s)
{

}