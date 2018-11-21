#include "featuremap.h"
#include "config.h"
#include "named-var-set.h"
#include "single-dimension-grid.h"
#include "app.h"
#include "worlds/world.h"

/////////////////////////////////////////////////////////////////////////////////
//FeatureMap: common functionality to State/ActionFeatureMap derived classes
/////////////////////////////////////////////////////////////////////////////////
FeatureMap::FeatureMap(ConfigNode* pConfigNode)
{
	m_numFeaturesPerVariable = INT_PARAM(pConfigNode, "Num-Features-Per-Dimension", "Number of features per input variable", 20);
}

FeatureMap::FeatureMap(size_t numFeaturesPerVariable)
{
	m_numFeaturesPerVariable.set((int) numFeaturesPerVariable);
}

size_t FeatureMap::getNumFeaturesPerVariable()
{
	return m_numFeaturesPerVariable.get();
}

size_t FeatureMap::getTotalNumFeatures() const
{
	return m_featureMapper->getTotalNumFeatures();
}

size_t FeatureMap::getMaxNumActiveFeatures() const
{
	return m_featureMapper->getMaxNumActiveFeatures();
}

void FeatureMap::getFeatures(const State* s, const Action* a, FeatureList* outFeatures)
{
	//copy input variable values to the internal buffer
	for (size_t grid = 0; grid < m_grids.size(); grid++)
		m_variableValues[grid] = getInputVariableValue(grid, s, a);

	//pass the buffer to the feature mapper
	m_featureMapper->map(m_grids, m_variableValues, outFeatures);
}

void FeatureMap::getFeatureStateAction(size_t feature, State* s, Action* a)
{
	//get the unmapped values in the internal buffer
	m_featureMapper->unmap(feature, m_grids, m_variableValues);

	//copy output values to the state/action
	for (size_t grid = 0; grid < m_grids.size(); grid++)
		setInputVariableValue(grid, m_variableValues[grid], s, a);
}


std::shared_ptr<FeatureMapper> FeatureMapper::getInstance(ConfigNode* pConfigNode)
{
	return CHOICE<FeatureMapper>(pConfigNode, "Type", "Feature map type",
		{
			{ "Discrete-Grid", CHOICE_ELEMENT_NEW<DiscreteFeatureMap> },
			{ "Gaussian-RBF-Grid", CHOICE_ELEMENT_NEW<GaussianRBFGridFeatureMap> },
			{ "Tile-Coding", CHOICE_ELEMENT_NEW<TileCodingFeatureMap> }
		});
}




/////////////////////////////////////////////////////////////
//StateFeatureMap
//////////////////////////////////////////////////////////////
StateFeatureMap::StateFeatureMap()
	:FeatureMap((size_t) 0)
{
}

StateFeatureMap::StateFeatureMap(ConfigNode* pConfigNode)
	:FeatureMap(pConfigNode)
{
	m_stateVariables = MULTI_VALUE_VARIABLE<STATE_VARIABLE>(pConfigNode, "Input-State", "State variables used as input of the feature map");

	//add the names of the input state variables to the list
	for (unsigned int i = 0; i < m_stateVariables.size(); i++)
	{
		m_stateVariableNames.push_back(m_stateVariables[i]->get());
		NamedVarProperties* pProp = SimionApp::get()->pWorld->getDynamicModel()->getStateDescriptor().getProperties(m_stateVariables[i]->get());
		m_grids.push_back(new SingleDimensionGrid(getNumFeaturesPerVariable(), pProp->getMin(), pProp->getMax(), pProp->isCircular()));
	}
	m_variableValues = vector<double>(m_grids.size());

	m_featureMapper = CHILD_OBJECT_FACTORY<FeatureMapper>(pConfigNode, "Feature-Mapper", "The feature calculator used to map/unmap features");
	m_featureMapper->init(m_grids);
}

StateFeatureMap::StateFeatureMap(FeatureMapper* pFeatureMapper, Descriptor& stateDescriptor, vector<size_t> variableIds, size_t numFeaturesPerVariable)
	:FeatureMap(numFeaturesPerVariable)
{
	for (size_t varid : variableIds)
	{
		m_stateVariableNames.push_back( stateDescriptor[varid].getName() );
		m_stateVariables.add(new STATE_VARIABLE(stateDescriptor[varid].getName()));
		m_grids.push_back(new SingleDimensionGrid(getNumFeaturesPerVariable(), stateDescriptor[varid].getMin(), stateDescriptor[varid].getMax(), stateDescriptor[varid].isCircular()));
	}
	m_variableValues = vector<double>(m_grids.size());

	m_featureMapper.set(pFeatureMapper);
	m_featureMapper->init(m_grids);
}

double StateFeatureMap::getInputVariableValue(size_t inputIndex, const State* s, const Action* a)
{
	return s->get( m_stateVariables[inputIndex]->get() );
}

void StateFeatureMap::setInputVariableValue(size_t inputIndex, double value, State* s, Action* a)
{
	return s->set(m_stateVariables[inputIndex]->get(), value);
}


/////////////////////////////////////////////////////////////
//ActionFeatureMap
//////////////////////////////////////////////////////////////
ActionFeatureMap::ActionFeatureMap()
	:FeatureMap((size_t) 0)
{
}

ActionFeatureMap::ActionFeatureMap(ConfigNode* pConfigNode)
	:FeatureMap(pConfigNode)
{
	m_actionVariables = MULTI_VALUE_VARIABLE<ACTION_VARIABLE>(pConfigNode, "Input-Action", "Action variables used as input of the feature map");

	//add the names of the input action variables to the list
	for (unsigned int i = 0; i < m_actionVariables.size(); i++)
	{
		m_actionVariableNames.push_back(m_actionVariables[i]->get());
		NamedVarProperties* pProp = SimionApp::get()->pWorld->getDynamicModel()->getActionDescriptor().getProperties(m_actionVariables[i]->get());
		m_grids.push_back(new SingleDimensionGrid(getNumFeaturesPerVariable(), pProp->getMin(), pProp->getMax(), pProp->isCircular()));
	}
	m_variableValues = vector<double>(m_grids.size());

	m_featureMapper = CHILD_OBJECT_FACTORY<FeatureMapper>(pConfigNode, "Feature-Mapper", "The feature calculator used to map/unmap features");
	m_featureMapper->init(m_grids);
}

ActionFeatureMap::ActionFeatureMap(FeatureMapper* pFeatureMapper, Descriptor& actionDescriptor, vector<size_t> variableIds, size_t numFeaturesPerVariable)
	:FeatureMap(numFeaturesPerVariable)
{
	for (size_t varid : variableIds)
	{
		m_actionVariableNames.push_back(actionDescriptor[varid].getName());
		m_actionVariables.add(new ACTION_VARIABLE(actionDescriptor[varid].getName()));
		m_grids.push_back(new SingleDimensionGrid(getNumFeaturesPerVariable(), actionDescriptor[varid].getMin(), actionDescriptor[varid].getMax(), actionDescriptor[varid].isCircular()));
	}
	m_variableValues = vector<double>(m_grids.size());

	m_featureMapper.set(pFeatureMapper);
	m_featureMapper->init(m_grids);
}


double ActionFeatureMap::getInputVariableValue(size_t inputIndex, const State* s, const Action* a)
{
	return a->get(m_actionVariables[inputIndex]->get());
}

void ActionFeatureMap::setInputVariableValue(size_t inputIndex, double value, State* s, Action* a)
{
	return a->set(m_actionVariables[inputIndex]->get(), value);
}


