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

#include "featuremap.h"
#include "config.h"
#include "../Common/named-var-set.h"
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

/// <summary>
/// Calculates the features for any given state-action
/// </summary>
/// <param name="s">State</param>
/// <param name="a">Action</param>
/// <param name="outFeatures">Output feature list</param>
void FeatureMap::getFeatures(const State* s, const Action* a, FeatureList* outFeatures)
{
	//copy input variable values to the internal buffer
	for (size_t grid = 0; grid < m_grids.size(); grid++)
		m_variableValues[grid] = getInputVariableValue(grid, s, a);

	//pass the buffer to the feature mapper
	m_featureMapper->map(m_grids, m_variableValues, outFeatures);
}


/// <summary>
/// Given a feature index, this method returns the state-action to which the feature corresponds. If the feature
/// map uses only states, the output action is left unmodified
/// </summary>
/// <param name="feature">Index of the feature</param>
/// <param name="s">Output state</param>
/// <param name="a">Output action</param>
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


