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
};

class ActionFeatureMap : public FeatureMap
{
public:
	ActionFeatureMap(ConfigNode* pParameters);
};

//Grid of Gaussian Radial Basis Functions///////////////////////
/////////////////////////////////////////////////////////////////
class GaussianRBFGridFeatureMap: public FeatureMap
{
	const size_t m_maxNumActiveFeaturesPerDimension = 3;
	FeatureList* m_pVarFeatures;

	INT_PARAM m_numFeaturesPerVariable;

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
	double m_tilingOffset;

	vector<SingleDimensionGrid*> m_grids;

	INT_PARAM m_numTiles;
	DOUBLE_PARAM m_tileOffset;
	INT_PARAM m_numFeaturesPerTile;

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
	FeatureList* m_pVarFeatures;

	vector<SingleDimensionGrid*> m_grids;

	DiscreteFeatureMap(ConfigNode* pParameters);
public:
	virtual ~DiscreteFeatureMap();

	void getFeatures(const State* s, const Action* a, FeatureList* outFeatures);
	void getFeatureStateAction(unsigned int feature, State* s, Action* a);
};


//
//class DiscreteStateFeatureMap : public DiscreteFeatureMap<SingleDimensionDiscreteStateVariableGrid>
//	, public StateFeatureMap
//{
//public:
//	DiscreteStateFeatureMap(ConfigNode* pParameters);
//
//	void getFeatures(const State* s, FeatureList* outFeatures) { DiscreteFeatureMap::getFeatures(s, 0, outFeatures); }
//	void getFeatureState(unsigned int feature, State* s) { DiscreteFeatureMap::getFeatureStateAction(feature, s, 0); }
//	size_t getTotalNumFeatures() { return m_totalNumFeatures; }
//	size_t getMaxNumActiveFeatures() { return m_maxNumActiveFeatures; }
//};
//
//class DiscreteActionFeatureMap : public DiscreteFeatureMap<SingleDimensionDiscreteActionVariableGrid>
//	, public ActionFeatureMap
//{
//public:
//	DiscreteActionFeatureMap(ConfigNode* pParameters);
//
//	void getFeatures(const Action* a, FeatureList* outFeatures) { DiscreteFeatureMap::getFeatures(0, a, outFeatures); }
//	void getFeatureAction(unsigned int feature, Action* a) { DiscreteFeatureMap::getFeatureStateAction(feature, 0, a); }
//	size_t getTotalNumFeatures() { return m_totalNumFeatures; }
//	size_t getMaxNumActiveFeatures() { return m_maxNumActiveFeatures; }
//};
//
////CBagFeatureMap////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
//class BagStateFeatureMap : public StateFeatureMap
//{
//	MULTI_VALUE_VARIABLE<STATE_VARIABLE> m_stateVariables;
//public:
//	BagStateFeatureMap(ConfigNode* pParameters);
//
//	void getFeatures(const State* s, FeatureList* outFeatures);
//	void getFeatureState(unsigned int feature, State* s);
//	size_t getTotalNumFeatures() { return m_stateVariables.size(); }
//	size_t getMaxNumActiveFeatures() { return m_stateVariables.size(); }
//};
//
//class BagActionFeatureMap : public ActionFeatureMap
//{
//	MULTI_VALUE_VARIABLE<ACTION_VARIABLE> m_actionVariables;
//public:
//	BagActionFeatureMap(ConfigNode* pParameters);
//
//	void getFeatures(const Action* a, FeatureList* outFeatures);
//	void getFeatureAction(unsigned int feature, Action* a);
//	size_t getTotalNumFeatures() { return m_actionVariables.size(); }
//	size_t getMaxNumActiveFeatures() { return m_actionVariables.size(); }
//};