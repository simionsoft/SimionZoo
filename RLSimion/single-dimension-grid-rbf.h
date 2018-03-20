#pragma once
class NamedVarSet;
typedef NamedVarSet State;
typedef NamedVarSet Action;
class FeatureList;
class ConfigNode;
#include "parameters.h"
#include "named-var-set.h"

#define ACTIVATION_THRESHOLD 0.0001

class SingleDimensionGridRBF
{
protected:
	INT_PARAM m_numCenters;
	double *m_pCenters;

	double m_min;
	double m_max;
	ENUM_PARAM<Distribution> m_distributionType;

	SingleDimensionGridRBF();

public:
	virtual ~SingleDimensionGridRBF();

	virtual void initVarRange()= 0;
	void initCenterPoints();

	int getNumCenters(){ return m_numCenters.get(); }
	double* getCenters(){ return m_pCenters; }

	void getFeatures(const State* s, const Action* a, FeatureList* outDimFeatures);
	double getFeatureFactor(int feature, double value);

	virtual double getVarValue(const State* s, const Action* a) = 0;
	virtual NamedVarProperties& getVarProperties(const State* s, const Action* a) = 0;
	virtual void setFeatureStateAction(unsigned int feature, State* s, Action* a) = 0;
};

class StateVariableGridRBF : public SingleDimensionGridRBF
{
	STATE_VARIABLE m_hVariable;
public:
	void initVarRange();
	StateVariableGridRBF(int m_hVar, int numCenters, Distribution distr= Distribution::linear);
	StateVariableGridRBF(ConfigNode* pParameters);
	double getVarValue(const State* s, const Action* a);
	NamedVarProperties& getVarProperties(const State* s, const Action* a);
	void setFeatureStateAction(unsigned int feature, State* s, Action* a);
};

class ActionVariableGridRBF : public SingleDimensionGridRBF
{
	ACTION_VARIABLE m_hVariable;
public:
	void initVarRange();
	ActionVariableGridRBF(int m_hVar, int numCenters, Distribution distr= Distribution::linear);
	ActionVariableGridRBF(ConfigNode* pParameters);
	double getVarValue(const State* s, const Action* a);
	NamedVarProperties& getVarProperties(const State* s, const Action* a);
	void setFeatureStateAction(unsigned int feature, State* s, Action* a);
};