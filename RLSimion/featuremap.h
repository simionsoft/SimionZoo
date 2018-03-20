#pragma once

class FeatureList;
class NamedVarSet;
typedef NamedVarSet State;
typedef NamedVarSet Action;
class ConfigNode;

#include "parameters.h"
#include "single-dimension-grid-rbf.h"
#include "single-dimension-grid.h"
#include "single-dimension-discrete-grid.h"
#include <vector>

//StateFeatureMap//////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
class StateFeatureMap
{
	ConfigNode* m_pParameters;
public:
	StateFeatureMap(ConfigNode* pParameters);
	virtual ~StateFeatureMap() {};

	virtual void getFeatures(const State* s, FeatureList* outFeatures) = 0;
	virtual void getFeatureState(unsigned int feature, State* s) = 0;

	virtual size_t getTotalNumFeatures() = 0;
	virtual size_t getMaxNumActiveFeatures() = 0;

	static std::shared_ptr<StateFeatureMap> getInstance(ConfigNode* pParameters);

	ConfigNode* getParameters() { return m_pParameters; }
};

class ActionFeatureMap
{
	ConfigNode* m_pParameters;
public:
	ActionFeatureMap(ConfigNode* pParameters);
	virtual ~ActionFeatureMap() {};

	virtual void getFeatures(const Action* a, FeatureList* outFeatures) = 0;
	virtual void getFeatureAction(unsigned int feature, Action* a) = 0;

	virtual size_t getTotalNumFeatures() = 0;
	virtual size_t getMaxNumActiveFeatures() = 0;

	static std::shared_ptr<ActionFeatureMap> getInstance(ConfigNode* pParameters);

	ConfigNode* getParameters() { return m_pParameters; }
};


//CGaussianRBFFeatureMap////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
#define MAX_NUM_ACTIVE_FEATURES_PER_DIMENSION 3
template <typename dimensionGridType>
class GaussianRBFGridFeatureMap
{
protected:
	FeatureList* m_pVarFeatures;

	size_t m_totalNumFeatures;
	size_t m_maxNumActiveFeatures;

	MULTI_VALUE<dimensionGridType> m_grid;

	GaussianRBFGridFeatureMap(ConfigNode* pParameters);
public:

	virtual ~GaussianRBFGridFeatureMap();

	void getFeatures(const State* s, const Action* a, FeatureList* outFeatures);
	void getFeatureStateAction(unsigned int feature, State* s, Action* a);

};

class GaussianRBFStateGridFeatureMap : public GaussianRBFGridFeatureMap<StateVariableGridRBF>
	, public StateFeatureMap
{
public:
	GaussianRBFStateGridFeatureMap(ConfigNode* pParameters);

	void getFeatures(const State* s, FeatureList* outFeatures) { GaussianRBFGridFeatureMap::getFeatures(s, 0, outFeatures); }
	void getFeatureState(unsigned int feature, State* s) { GaussianRBFGridFeatureMap::getFeatureStateAction(feature, s, 0); }
	size_t getTotalNumFeatures() { return m_totalNumFeatures; }
	size_t getMaxNumActiveFeatures() { return m_maxNumActiveFeatures; }

};

class GaussianRBFActionGridFeatureMap : public GaussianRBFGridFeatureMap<ActionVariableGridRBF>
	, public ActionFeatureMap
{
public:
	GaussianRBFActionGridFeatureMap(ConfigNode* pParameters);

	void getFeatures(const Action* a, FeatureList* outFeatures) { GaussianRBFGridFeatureMap::getFeatures(0, a, outFeatures); }
	void getFeatureAction(unsigned int feature, Action* a) { GaussianRBFGridFeatureMap::getFeatureStateAction(feature, 0, a); }
	size_t getTotalNumFeatures() { return m_totalNumFeatures; }
	size_t getMaxNumActiveFeatures() { return m_maxNumActiveFeatures; }

};

//TileCodingFeatureMap////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
template <typename dimensionGridType>
class TileCodingFeatureMap
{
protected:
	FeatureList* m_pVarFeatures;

	size_t m_totalNumFeatures;
	size_t m_maxNumActiveFeatures;
	size_t m_numTilings;

	//modified states s and actions a which are "moved" by the offset of the layer
	const State* modified_s;
	const State* modified_a;


	double m_tilingOffset;

	MULTI_VALUE<dimensionGridType> m_grid;

	TileCodingFeatureMap(ConfigNode* pParameters);
public:

	virtual ~TileCodingFeatureMap();

	void getFeatures(const State* s, const Action* a, FeatureList* outFeatures);
	void getFeatureStateAction(unsigned int feature, State* s, Action* a);
	virtual size_t getNumTilings() = 0;

	MULTI_VALUE<dimensionGridType> returnGrid() { return m_grid; }

};

class TileCodingStateFeatureMap : public TileCodingFeatureMap<SingleDimensionStateVariableGrid>
	, public StateFeatureMap
{
public:
	TileCodingStateFeatureMap(ConfigNode* pParameters);

	void getFeatures(const State* s, FeatureList* outFeatures) { TileCodingFeatureMap::getFeatures(s, 0, outFeatures); }
	void getFeatureState(unsigned int feature, State* s) { TileCodingFeatureMap::getFeatureStateAction(feature, s, 0); }
	size_t getTotalNumFeatures() { return m_totalNumFeatures; }
	size_t getMaxNumActiveFeatures() { return m_maxNumActiveFeatures; }
	size_t getNumTilings() { return m_numTilings; }
	double getTilingOffset() { return m_tilingOffset; }
};

class TileCodingActionFeatureMap : public TileCodingFeatureMap<SingleDimensionActionVariableGrid>
	, public ActionFeatureMap
{
public:
	TileCodingActionFeatureMap(ConfigNode* pParameters);

	void getFeatures(const Action* a, FeatureList* outFeatures) { TileCodingFeatureMap::getFeatures(0, a, outFeatures); }
	void getFeatureAction(unsigned int feature, Action* a) { TileCodingFeatureMap::getFeatureStateAction(feature, 0, a); }
	size_t getTotalNumFeatures() { return m_totalNumFeatures; }
	size_t getMaxNumActiveFeatures() { return m_maxNumActiveFeatures; }
	size_t getNumTilings() { return m_numTilings; }
	double getTilingOffset() { return m_tilingOffset; }
};

//DiscreteFeatureMap////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
template <typename dimensionGridType>
class DiscreteFeatureMap
{
protected:
	FeatureList* m_pVarFeatures;

	size_t m_totalNumFeatures;
	size_t m_maxNumActiveFeatures;

	MULTI_VALUE<dimensionGridType> m_grid;

	DiscreteFeatureMap(ConfigNode* pParameters);
public:
	virtual ~DiscreteFeatureMap();

	void getFeatures(const State* s, const Action* a, FeatureList* outFeatures);
	void getFeatureStateAction(unsigned int feature, State* s, Action* a);

	MULTI_VALUE<dimensionGridType> returnGrid() { return m_grid; }
};

class DiscreteStateFeatureMap : public DiscreteFeatureMap<SingleDimensionDiscreteStateVariableGrid>
	, public StateFeatureMap
{
public:
	DiscreteStateFeatureMap(ConfigNode* pParameters);

	void getFeatures(const State* s, FeatureList* outFeatures) { DiscreteFeatureMap::getFeatures(s, 0, outFeatures); }
	void getFeatureState(unsigned int feature, State* s) { DiscreteFeatureMap::getFeatureStateAction(feature, s, 0); }
	size_t getTotalNumFeatures() { return m_totalNumFeatures; }
	size_t getMaxNumActiveFeatures() { return m_maxNumActiveFeatures; }
};

class DiscreteActionFeatureMap : public DiscreteFeatureMap<SingleDimensionDiscreteActionVariableGrid>
	, public ActionFeatureMap
{
public:
	DiscreteActionFeatureMap(ConfigNode* pParameters);

	void getFeatures(const Action* a, FeatureList* outFeatures) { DiscreteFeatureMap::getFeatures(0, a, outFeatures); }
	void getFeatureAction(unsigned int feature, Action* a) { DiscreteFeatureMap::getFeatureStateAction(feature, 0, a); }
	size_t getTotalNumFeatures() { return m_totalNumFeatures; }
	size_t getMaxNumActiveFeatures() { return m_maxNumActiveFeatures; }
};

//CBagFeatureMap////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
class BagStateFeatureMap : public StateFeatureMap
{
	MULTI_VALUE_VARIABLE<STATE_VARIABLE> m_stateVariables;
public:
	BagStateFeatureMap(ConfigNode* pParameters);

	void getFeatures(const State* s, FeatureList* outFeatures);
	void getFeatureState(unsigned int feature, State* s);
	size_t getTotalNumFeatures() { return m_stateVariables.size(); }
	size_t getMaxNumActiveFeatures() { return m_stateVariables.size(); }
};

class BagActionFeatureMap : public ActionFeatureMap
{
	MULTI_VALUE_VARIABLE<ACTION_VARIABLE> m_actionVariables;
public:
	BagActionFeatureMap(ConfigNode* pParameters);

	void getFeatures(const Action* a, FeatureList* outFeatures);
	void getFeatureAction(unsigned int feature, Action* a);
	size_t getTotalNumFeatures() { return m_actionVariables.size(); }
	size_t getMaxNumActiveFeatures() { return m_actionVariables.size(); }
};