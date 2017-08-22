#pragma once
class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;
class CFeatureList;
class CConfigNode;
#include "parameters.h"
#include "named-var-set.h"

class CSingleDimensionDiscreteGrid
{
protected:
	DOUBLE_PARAM m_stepSize;
	double *m_pCenters;
	
	int m_numCenters;
	double m_min;
	double m_max;
	double m_offset = 0.0;

	CSingleDimensionDiscreteGrid();

public:
	virtual ~CSingleDimensionDiscreteGrid();

	virtual void initVarRange() = 0;
	void initCenterPoints();

	int getNumCenters() { return m_numCenters; }
	double* getCenters() { return m_pCenters; }

	double getStepSize() { return m_stepSize.get(); }

	void getFeatures(const CState* s, const CAction* a, CFeatureList* outDimFeatures);

	virtual double getVarValue(const CState* s, const CAction* a) = 0;
	virtual CNamedVarProperties& getVarProperties(const CState* s, const CAction* a) = 0;
	virtual void setFeatureStateAction(unsigned int feature, CState* s, CAction* a) = 0;

	double getOffset() { return m_offset; }
	virtual void setOffset(double offset) = 0;

	int getClosestCenter(double value);
};

class CSingleDimensionDiscreteStateVariableGrid : public CSingleDimensionDiscreteGrid
{
	STATE_VARIABLE m_hVariable;
public:
	void initVarRange();
	CSingleDimensionDiscreteStateVariableGrid(int m_hVar, double stepSize);
	CSingleDimensionDiscreteStateVariableGrid(CConfigNode* pParameters);
	double getVarValue(const CState* s, const CAction* a);
	CNamedVarProperties& getVarProperties(const CState* s, const CAction* a);
	void setFeatureStateAction(unsigned int feature, CState* s, CAction* a);
	void setOffset(double offset);
};

class CSingleDimensionDiscreteActionVariableGrid : public CSingleDimensionDiscreteGrid
{
	ACTION_VARIABLE m_hVariable;
public:
	void initVarRange();
	CSingleDimensionDiscreteActionVariableGrid(int m_hVar, double stepSize);
	CSingleDimensionDiscreteActionVariableGrid(CConfigNode* pParameters);
	double getVarValue(const CState* s, const CAction* a);
	CNamedVarProperties& getVarProperties(const CState* s, const CAction* a);
	void setFeatureStateAction(unsigned int feature, CState* s, CAction* a);
	void setOffset(double offset);
};