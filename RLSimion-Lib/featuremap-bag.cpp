#include "stdafx.h"
#include "featuremap.h"
#include "named-var-set.h"
#include "worlds/world.h"
#include "features.h"
#include "config.h"
#include "single-dimension-grid.h"

CBagStateFeatureMap::CBagStateFeatureMap(CConfigNode* pParameters) : CStateFeatureMap(pParameters)
{
	m_stateVariables = MULTI_VALUE_VARIABLE<STATE_VARIABLE>(pParameters, "State-Variables", "The state variables");
}

CBagActionFeatureMap::CBagActionFeatureMap(CConfigNode* pParameters) : CActionFeatureMap(pParameters)
{
	m_actionVariables = MULTI_VALUE_VARIABLE<ACTION_VARIABLE>(pParameters, "Action-Variables", "The action variables");
}

void CBagStateFeatureMap::getFeatures(const CState* s, CFeatureList* outFeatures)
{
	outFeatures->clear();
	for (size_t i = 0; i < m_stateVariables.size(); i++)
	{
		int index = m_stateVariables[i]->get();
		outFeatures->add(index, s->get(index));
	}
}

void CBagStateFeatureMap::getFeatureState(unsigned int feature, CState* s)
{

}

void CBagActionFeatureMap::getFeatures(const CAction* a, CFeatureList* outFeatures)
{
	outFeatures->clear();
	for (size_t i = 0; i < m_actionVariables.size(); i++)
	{
		int index = m_actionVariables[i]->get();
		outFeatures->add(index, a->get(index));
	}
}

void CBagActionFeatureMap::getFeatureAction(unsigned int feature, CAction* s)
{

}