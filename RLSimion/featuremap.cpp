#include "featuremap.h"
#include "config.h"
#include "app.h"
#include "named-var-set.h"

FeatureMap::FeatureMap(ConfigNode* pConfigNode)
{
	m_stateVariables = MULTI_VALUE_VARIABLE<STATE_VARIABLE>(pConfigNode, "State-Variables", "State variables used as input of the feature map");
	m_actionVariables = MULTI_VALUE_VARIABLE<ACTION_VARIABLE>(pConfigNode, "State-Variables", "State variables used as input of the feature map");
	
	//add the names of the input state variables to the list
	for (unsigned int i = 0; i < m_stateVariables.size(); i++)
		m_stateVariableNames.push_back(m_stateVariables[i]->getName());
	//add the names of the input action variables to the list
	for (unsigned int i = 0; i < m_actionVariables.size(); i++)
		m_actionVariableNames.push_back(m_actionVariables[i]->getName());
}


std::shared_ptr<FeatureMap> FeatureMap::getInstance(ConfigNode* pConfigNode)
{
	return CHOICE<FeatureMap>(pConfigNode, "Type", "Feature map type",
		{
			//{ "Discrete-State-Grid", CHOICE_ELEMENT_NEW<DiscreteFeatureMap> },
			{ "Gaussian-RBF-Grid", CHOICE_ELEMENT_NEW<GaussianRBFGridFeatureMap> },
			{ "Tile-Coding-State-Grid", CHOICE_ELEMENT_NEW<TileCodingFeatureMap> }
			//{ "State-Bag", CHOICE_ELEMENT_NEW<BagFeatureMap> }
		});
}

StateFeatureMap::StateFeatureMap(ConfigNode* pConfigNode)
{
	m_stateVariables = MULTI_VALUE_VARIABLE<STATE_VARIABLE>(pConfigNode, "State-Variables", "State variables used as input of the feature map");
	//add the names of the input state variables to the list
	for (unsigned int i = 0; i < m_stateVariables.size(); i++)
		m_stateVariableNames.push_back(m_stateVariables[i]->getName());
}

ActionFeatureMap::ActionFeatureMap(ConfigNode* pConfigNode)
{
	m_actionVariables = MULTI_VALUE_VARIABLE<ACTION_VARIABLE>(pConfigNode, "Action-Variables", "State variables used as input of the feature map");
	//add the names of the input action variables to the list
	for (unsigned int i = 0; i < m_actionVariables.size(); i++)
		m_actionVariableNames.push_back(m_actionVariables[i]->getName());
}

double FeatureMap::getInputVariableValue(size_t inputIndex, const State* s, const Action* a) const
{
	if (inputIndex < m_stateVariables.size())
		return s->get(inputIndex);
	return a->get(inputIndex - m_stateVariables.size());
}

void FeatureMap::setInputVariableValue(size_t inputIndex, double value, State* s, Action* a)
{
	if (inputIndex < m_stateVariables.size())
		return s->set(inputIndex, value);
	return a->set(inputIndex - m_stateVariables.size(), value);
}
//
//std::shared_ptr<StateFeatureMap> StateFeatureMap::getInstance(ConfigNode* pConfigNode)
//{
//	return CHOICE<StateFeatureMap>(pConfigNode, "Type", "Feature map type",
//	{
//		{"Discrete-State-Grid", CHOICE_ELEMENT_NEW<DiscreteStateFeatureMap>},
//		{"RBF-State-Grid", CHOICE_ELEMENT_NEW<GaussianRBFStateGridFeatureMap>},
//		{"Tile-Coding-State-Grid", CHOICE_ELEMENT_NEW<TileCodingStateFeatureMap>},
//		{"State-Bag", CHOICE_ELEMENT_NEW<BagStateFeatureMap>}
//	});
//}
//
//
//ActionFeatureMap::ActionFeatureMap(ConfigNode* pConfigNode)
//{
//}
//
//std::shared_ptr<ActionFeatureMap> ActionFeatureMap::getInstance(ConfigNode* pConfigNode)
//{
//	return CHOICE<ActionFeatureMap>(pConfigNode, "Type", "Feature map type",
//	{
//		{"Discrete-Action-Grid", CHOICE_ELEMENT_NEW<DiscreteActionFeatureMap>},
//		{"RBF-Action-Grid", CHOICE_ELEMENT_NEW<GaussianRBFActionGridFeatureMap>},
//		{"Tile-Coding-Action-Grid", CHOICE_ELEMENT_NEW<TileCodingActionFeatureMap>},
//		{"Action-Bag", CHOICE_ELEMENT_NEW<BagActionFeatureMap>}
//	});
//}
//
