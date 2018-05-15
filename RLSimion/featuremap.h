#pragma once

class FeatureList;
class NamedVarSet;
using State = NamedVarSet;
using Action = NamedVarSet;
class ConfigNode;
class SingleDimensionGrid;

#include "parameters.h"
#include <vector>
#include <string>

//FeatureMap: generic base class
////////////////////////////////////////////////////////////////////////
class FeatureMap
{
protected:
	vector<string> m_stateVariableNames;
	vector<string> m_actionVariableNames;
	MULTI_VALUE_VARIABLE<STATE_VARIABLE> m_stateVariables;
	MULTI_VALUE_VARIABLE<ACTION_VARIABLE> m_actionVariables;

	size_t m_totalNumFeatures;
	size_t m_maxNumActiveFeatures;
public:
	FeatureMap();
	FeatureMap(ConfigNode* pParameters);
	virtual ~FeatureMap() {};

	virtual void getFeatures(const State* s, const Action* a, FeatureList* outFeatures) = 0;
	virtual void getFeatureStateAction(size_t feature, State* s, Action* a) = 0;

	static std::shared_ptr<FeatureMap> getInstance(ConfigNode* pParameters);

	const vector<string>& getInputStateVariables() { return m_stateVariableNames; }
	const vector<string>& getInputActionVariables() { return m_actionVariableNames; }
	size_t getTotalNumFeatures() const { return m_totalNumFeatures; }
	size_t getMaxNumActiveFeatures() const { return m_maxNumActiveFeatures; }

	double getInputVariableValue(size_t inputIndex, const State* s, const Action* a) const;
	void setInputVariableValue(size_t inputIndex, double value, State* s, Action* a);
};

class StateFeatureMap : public FeatureMap
{
public:
	StateFeatureMap(ConfigNode* pParameters);

	static std::shared_ptr<StateFeatureMap> getInstance(ConfigNode* pParameters);
};

class ActionFeatureMap : public FeatureMap
{
public:
	ActionFeatureMap(ConfigNode* pParameters);

	static std::shared_ptr<ActionFeatureMap> getInstance(ConfigNode* pParameters);
};

//Grid of Gaussian Radial Basis Functions///////////////////////
/////////////////////////////////////////////////////////////////
class GaussianRBFGridFeatureMap: public FeatureMap
{
	INT_PARAM m_numFeaturesPerVariable;

	const size_t m_maxNumActiveFeaturesPerDimension = 3;
	FeatureList* m_pVarFeatures;
	vector<SingleDimensionGrid*> m_grids;

	double getFeatureFactor(size_t varIndex, size_t feature, double value) const;
	void getDimensionFeatures(size_t varIndex, const State* s, const Action* a, FeatureList* outFeatures) const;
public:
	GaussianRBFGridFeatureMap(ConfigNode* pParameters);
	virtual ~GaussianRBFGridFeatureMap();

	void getFeatures(const State* s, const Action* a, FeatureList* outFeatures);
	void getFeatureStateAction(size_t feature, State* s, Action* a);
};


//Tile coding////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
class TileCodingFeatureMap: public FeatureMap
{
protected:
	INT_PARAM m_numTiles;
	DOUBLE_PARAM m_tileOffset;
	INT_PARAM m_numFeaturesPerTile;

	vector<SingleDimensionGrid*> m_grids;
public:
	TileCodingFeatureMap(ConfigNode* pParameters);
	virtual ~TileCodingFeatureMap();

	void getFeatures(const State* s, const Action* a, FeatureList* outFeatures);
	void getFeatureStateAction(size_t feature, State* s, Action* a);
};


//DiscreteFeatureMap////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
class DiscreteFeatureMap: public FeatureMap
{
protected:
	INT_PARAM m_numFeaturesPerVariable;

	vector<SingleDimensionGrid*> m_grids;
public:
	DiscreteFeatureMap(ConfigNode* pParameters);
	virtual ~DiscreteFeatureMap();

	void getFeatures(const State* s, const Action* a, FeatureList* outFeatures);
	void getFeatureStateAction(size_t feature, State* s, Action* a);
};
