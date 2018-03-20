#pragma once
class NamedVarSet;
typedef NamedVarSet State;
typedef NamedVarSet Action;
class FeatureList;
class ConfigNode;
#include "parameters.h"
#include "named-var-set.h"

class SingleDimensionGrid
{
protected:
	INT_PARAM m_numCenters;
	double *m_pCenters;

	double m_min;
	double m_max;
	double m_offset = 0.0;

	ENUM_PARAM<Distribution> m_distributionType;

	SingleDimensionGrid();

public:
	virtual ~SingleDimensionGrid();

	virtual void initVarRange() = 0;
	void initCenterPoints();

	int getNumCenters() { return m_numCenters.get(); }
	double* getCenters() { return m_pCenters; }

	void getFeatures(const State* s, const Action* a, FeatureList* outDimFeatures);

	virtual double getVarValue(const State* s, const Action* a) = 0;
	virtual NamedVarProperties& getVarProperties(const State* s, const Action* a) = 0;
	virtual void setFeatureStateAction(unsigned int feature, State* s, Action* a) = 0;

	double getOffset() { return m_offset; }
	virtual void setOffset(double offset) = 0;

	int getClosestCenter(double value);
};

class SingleDimensionStateVariableGrid : public SingleDimensionGrid
{
	STATE_VARIABLE m_hVariable;
public:
	void initVarRange();
	SingleDimensionStateVariableGrid(int m_hVar, int numCenters, Distribution distr = Distribution::linear);
	SingleDimensionStateVariableGrid(ConfigNode* pParameters);
	double getVarValue(const State* s, const Action* a);
	NamedVarProperties& getVarProperties(const State* s, const Action* a);
	void setFeatureStateAction(unsigned int feature, State* s, Action* a);
	void setOffset(double offset);
};

class SingleDimensionActionVariableGrid : public SingleDimensionGrid
{
	ACTION_VARIABLE m_hVariable;
public:
	void initVarRange();
	SingleDimensionActionVariableGrid(int m_hVar, int numCenters, Distribution distr = Distribution::linear);
	SingleDimensionActionVariableGrid(ConfigNode* pParameters);
	double getVarValue(const State* s, const Action* a);
	NamedVarProperties& getVarProperties(const State* s, const Action* a);
	void setFeatureStateAction(unsigned int feature, State* s, Action* a);
	void setOffset(double offset);
};