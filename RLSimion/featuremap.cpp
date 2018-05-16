#include "featuremap.h"
#include "config.h"
#include "app.h"
#include "named-var-set.h"

FeatureMap::FeatureMap()
{
}

FeatureMap::FeatureMap(ConfigNode* pConfigNode)
{
}


std::shared_ptr<FeatureMap> FeatureMap::getInstance(ConfigNode* pConfigNode)
{
	return CHOICE<FeatureMap>(pConfigNode, "Type", "Feature map type",
		{
			{ "Discrete-State-Grid", CHOICE_ELEMENT_NEW<DiscreteFeatureMap> },
			{ "Gaussian-RBF-Grid", CHOICE_ELEMENT_NEW<GaussianRBFGridFeatureMap> },
			{ "Tile-Coding-State-Grid", CHOICE_ELEMENT_NEW<TileCodingFeatureMap> }
		});
}

double FeatureMap::getInputVariableValue(size_t inputIndex, const State* s, const Action* a)
{
	if (inputIndex < m_stateVariables.size())
		return s->get( m_stateVariables[inputIndex]->get() );
	return a->get( m_actionVariables[inputIndex - m_stateVariables.size()]->get());
}

void FeatureMap::setInputVariableValue(size_t inputIndex, double value, State* s, Action* a)
{
	if (inputIndex < m_stateVariables.size())
		return s->set(m_stateVariables[inputIndex]->get(), value);
	return a->set(m_actionVariables[inputIndex - m_stateVariables.size()]->get(), value);
}


std::shared_ptr<StateFeatureMap> StateFeatureMap::getInstance(ConfigNode* pConfigNode)
{
	std::shared_ptr<FeatureMap> pFeatureMap = FeatureMap::getInstance(pConfigNode);
	return std::shared_ptr<StateFeatureMap>(std::static_pointer_cast<StateFeatureMap>(pFeatureMap));
}
StateFeatureMap::StateFeatureMap(Descriptor& stateDescriptor, vector<size_t> variableIds)
{
	for each(size_t varid in variableIds)
		m_stateVariables.add(new STATE_VARIABLE(stateDescriptor, varid));
}

std::shared_ptr<ActionFeatureMap> ActionFeatureMap::getInstance(ConfigNode* pConfigNode)
{
	std::shared_ptr<FeatureMap> pFeatureMap = FeatureMap::getInstance(pConfigNode);
	return std::shared_ptr<ActionFeatureMap>(std::static_pointer_cast<ActionFeatureMap>(pFeatureMap));
}
ActionFeatureMap::ActionFeatureMap(Descriptor& actionDescriptor, vector<size_t> variableIds)
{
	for each(size_t varid in variableIds)
		m_actionVariables.add(new ACTION_VARIABLE(actionDescriptor, varid));
}



StateFeatureMap::StateFeatureMap(ConfigNode* pConfigNode)
	:FeatureMap(pConfigNode)
{
	m_stateVariables = MULTI_VALUE_VARIABLE<STATE_VARIABLE>(pConfigNode, "State-Variables", "State variables used as input of the feature map");
	//add the names of the input state variables to the list
	for (unsigned int i = 0; i < m_stateVariables.size(); i++)
		m_stateVariableNames.push_back(m_stateVariables[i]->getName());
}



ActionFeatureMap::ActionFeatureMap(ConfigNode* pConfigNode)
	:FeatureMap(pConfigNode)
{
	m_actionVariables = MULTI_VALUE_VARIABLE<ACTION_VARIABLE>(pConfigNode, "Action-Variables", "State variables used as input of the feature map");
	//add the names of the input action variables to the list
	for (unsigned int i = 0; i < m_actionVariables.size(); i++)
		m_actionVariableNames.push_back(m_actionVariables[i]->getName());
}



