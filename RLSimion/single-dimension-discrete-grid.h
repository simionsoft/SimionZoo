#pragma once
class NamedVarSet;
typedef NamedVarSet State;
typedef NamedVarSet Action;
class FeatureList;
class ConfigNode;
#include "parameters.h"
#include "named-var-set.h"

class SingleDimensionDiscreteGrid
{
protected:
	INT_PARAM m_numCenters;
	double *m_pCenters;
	
	double m_stepSize;
	double m_min;
	double m_max;
	double m_offset = 0.0;

	SingleDimensionDiscreteGrid();

public:
	virtual ~SingleDimensionDiscreteGrid();

	virtual void initVarRange() = 0;
	void initCenterPoints();

	int getNumCenters() { return m_numCenters.get(); }
	double* getCenters() { return m_pCenters; }

	double getStepSize() { return m_stepSize; }

	void getFeatures(const State* s, const Action* a, FeatureList* outDimFeatures);

	virtual double getVarValue(const State* s, const Action* a) = 0;
	virtual NamedVarProperties& getVarProperties(const State* s, const Action* a) = 0;
	virtual void setFeatureStateAction(unsigned int feature, State* s, Action* a) = 0;

	double getOffset() { return m_offset; }
	virtual void setOffset(double offset) = 0;

	int getClosestCenter(double value);
};

class SingleDimensionDiscreteStateVariableGrid : public SingleDimensionDiscreteGrid
{
	STATE_VARIABLE m_hVariable;
public:
	void initVarRange();
	SingleDimensionDiscreteStateVariableGrid(int m_hVar, int steps);
	SingleDimensionDiscreteStateVariableGrid(ConfigNode* pParameters);
	double getVarValue(const State* s, const Action* a);
	NamedVarProperties& getVarProperties(const State* s, const Action* a);
	void setFeatureStateAction(unsigned int feature, State* s, Action* a);
	void setOffset(double offset);
};

class SingleDimensionDiscreteActionVariableGrid : public SingleDimensionDiscreteGrid
{
	ACTION_VARIABLE m_hVariable;
public:
	void initVarRange();
	SingleDimensionDiscreteActionVariableGrid(int m_hVar, int steps);
	SingleDimensionDiscreteActionVariableGrid(ConfigNode* pParameters);
	double getVarValue(const State* s, const Action* a);
	NamedVarProperties& getVarProperties(const State* s, const Action* a);
	void setFeatureStateAction(unsigned int feature, State* s, Action* a);
	void setOffset(double offset);
};