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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//FeatureMapper: base class for feature mappers
//Feature mappers implement the actual mapping between states/actions to feature sparse vectors and unmaps them
//without holding the information about the state/action variables and the grid. The grids are kept by the parent
//FeatureMap object and, on mapping, passes the grid and the value of each variable
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class FeatureMapper
{
public:
	virtual void init(vector<SingleDimensionGrid*>& outGrids) = 0;
	virtual void map(vector<SingleDimensionGrid*>& grids, const vector<double>& values, FeatureList* outFeatures) = 0;
	virtual void unmap(size_t feature, vector<SingleDimensionGrid*>& grids, vector<double>& outValues) = 0;

	virtual size_t getTotalNumFeatures() const = 0;
	virtual size_t getMaxNumActiveFeatures() const = 0;

	static std::shared_ptr<FeatureMapper> getInstance(ConfigNode* pConfigNode);
};


//GaussianRBFGridFeatureMap implements a grid of Gaussian Radial Basis Functions///////////////////////
class GaussianRBFGridFeatureMap : public FeatureMapper
{
	size_t m_totalNumFeatures;
	size_t m_maxNumActiveFeatures;
	const size_t m_maxNumActiveFeaturesPerDimension = 3;
	FeatureList* m_pVarFeatures;

	double getFeatureFactor(SingleDimensionGrid* pGrid, size_t feature, double value) const;
	void getDimensionFeatures(SingleDimensionGrid* pGrid, double value, FeatureList* outFeatures);
public:
	GaussianRBFGridFeatureMap();
	GaussianRBFGridFeatureMap(ConfigNode* pParameters);
	virtual ~GaussianRBFGridFeatureMap();

	void init(vector<SingleDimensionGrid*>& grids);
	void map(vector<SingleDimensionGrid*>& grids, const vector<double>& values, FeatureList* outFeatures);
	void unmap(size_t feature, vector<SingleDimensionGrid*>& grids, vector<double>& outValues);

	size_t getTotalNumFeatures() const { return m_totalNumFeatures; };
	size_t getMaxNumActiveFeatures() const { return m_maxNumActiveFeatures; };
};


//Tile coding////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
class TileCodingFeatureMap : public FeatureMapper
{
protected:
	INT_PARAM m_numTiles;
	DOUBLE_PARAM m_tileOffset;
	size_t m_numFeaturesPerTile;
	size_t m_totalNumFeatures;
	size_t m_maxNumActiveFeatures;
public:
	TileCodingFeatureMap(size_t numTiles, double tileOffset);
	TileCodingFeatureMap(ConfigNode* pParameters);
	virtual ~TileCodingFeatureMap();

	void init(vector<SingleDimensionGrid*>& grids);
	void map(vector<SingleDimensionGrid*>& grids, const vector<double>& values, FeatureList* outFeatures);
	void unmap(size_t feature, vector<SingleDimensionGrid*>& grids, vector<double>& outValues);

	size_t getTotalNumFeatures() const { return m_totalNumFeatures; };
	size_t getMaxNumActiveFeatures() const { return m_maxNumActiveFeatures; };
};


//DiscreteFeatureMap////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
class DiscreteFeatureMap : public FeatureMapper
{
protected:
	size_t m_totalNumFeatures;
	size_t m_maxNumActiveFeatures;

public:
	DiscreteFeatureMap();
	DiscreteFeatureMap(ConfigNode* pParameters);
	virtual ~DiscreteFeatureMap();

	void init(vector<SingleDimensionGrid*>& grids);
	void map(vector<SingleDimensionGrid*>& grids, const vector<double>& values, FeatureList* outFeatures);
	void unmap(size_t feature, vector<SingleDimensionGrid*>& grids, vector<double>& outValues);

	size_t getTotalNumFeatures() const { return m_totalNumFeatures; };
	size_t getMaxNumActiveFeatures() const { return m_maxNumActiveFeatures; };
};


////////////////////////////////////////////////////////////////////////////////////////////////
//FeatureMap: generic base class from which derive StateFeatureMap and ActionFeatureMap
//Basically, it handles the input state/action data and a pointer to a FeatureMapper object that
//does the actual mapping
/////////////////////////////////////////////////////////////////////////////////////////////////
class FeatureMap
{
	INT_PARAM m_numFeaturesPerVariable;
protected:
	vector<SingleDimensionGrid*> m_grids;
	vector<double> m_variableValues;

	CHILD_OBJECT_FACTORY<FeatureMapper> m_featureMapper;

	MULTI_VALUE_VARIABLE<STATE_VARIABLE> m_stateVariables;
	MULTI_VALUE_VARIABLE<ACTION_VARIABLE> m_actionVariables;
	vector<string> m_stateVariableNames;
	vector<string> m_actionVariableNames;

	//protected constructors to avoid instances of FeatureMap. Subclasses should be used
	FeatureMap(size_t numFeaturesPerVariable);
	FeatureMap(ConfigNode* pConfigNode);

	size_t getNumFeaturesPerVariable();
public:
	virtual ~FeatureMap() {};

	size_t getTotalNumFeatures() const;
	size_t getMaxNumActiveFeatures() const;

	void getFeatures(const State* s, const Action* a, FeatureList* outFeatures);
	void getFeatureStateAction(size_t feature, State* s, Action* a);

	virtual double getInputVariableValue(size_t inputIndex, const State* s, const Action* a) = 0;
	virtual void setInputVariableValue(size_t inputIndex, double value, State* s, Action* a) = 0;

	vector<string>& getInputStateVariables() { return m_stateVariableNames; }
	vector<string>& getInputActionVariables() { return m_actionVariableNames; }
};

//We distinguish these two feature maps to make sure that only the right subset of variables (state or action variables)
//is selected as input
class StateFeatureMap: public FeatureMap
{
public:
	//Constructor needed for optional parameters
	StateFeatureMap();
	//Constructor used for testing. No config file used
	StateFeatureMap(FeatureMapper* pFeatureMapper, Descriptor& stateDescriptor, vector<size_t> variableIds, size_t numFeaturesPerVariable);
	//Constructor used when a config file is used
	StateFeatureMap(ConfigNode* pParameters);
	virtual ~StateFeatureMap() {};
	double getInputVariableValue(size_t inputIndex, const State* s, const Action* a);
	void setInputVariableValue(size_t inputIndex, double value, State* s, Action* a);
};

class ActionFeatureMap: public FeatureMap
{
public:
	//Constructor needed for optional parameters
	ActionFeatureMap();
	//Constructor used for testing. No config file used
	ActionFeatureMap(FeatureMapper* pFeatureMapper, Descriptor& stateDescriptor, vector<size_t> variableIds, size_t numFeaturesPerVariable);
	//Constructor used when a config file is used
	ActionFeatureMap(ConfigNode* pParameters);
	virtual ~ActionFeatureMap() {};
	double getInputVariableValue(size_t inputIndex, const State* s, const Action* a);
	void setInputVariableValue(size_t inputIndex, double value, State* s, Action* a);
};


