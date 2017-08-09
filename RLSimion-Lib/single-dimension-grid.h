#pragma once
class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;
class CFeatureList;
class CConfigNode;
#include "parameters.h"
#include "named-var-set.h"

class CSingleDimensionGrid
{
protected:
	INT_PARAM m_numCenters;
	double *m_pCenters;

	double m_min;
	double m_max;
	double m_offset = 0.0;

	ENUM_PARAM<Distribution> m_distributionType;

	CSingleDimensionGrid();

public:
	virtual ~CSingleDimensionGrid();

	virtual void initVarRange() = 0;
	void initCenterPoints();

	int getNumCenters() { return m_numCenters.get(); }
	double* getCenters() { return m_pCenters; }

	void getFeatures(const CState* s, const CAction* a, CFeatureList* outDimFeatures);

	virtual double getVarValue(const CState* s, const CAction* a) = 0;
	virtual CNamedVarProperties& getVarProperties(const CState* s, const CAction* a) = 0;
	virtual void setFeatureStateAction(unsigned int feature, CState* s, CAction* a) = 0;

	double getOffset() { return m_offset; }
	virtual void setOffset(double offset) = 0;
};

class CStateVariableGrid : public CSingleDimensionGrid
{
	STATE_VARIABLE m_hVariable;
public:
	void initVarRange();
	CStateVariableGrid(int m_hVar, int numCenters, Distribution distr = Distribution::linear);
	CStateVariableGrid(CConfigNode* pParameters);
	double getVarValue(const CState* s, const CAction* a);
	CNamedVarProperties& getVarProperties(const CState* s, const CAction* a);
	void setFeatureStateAction(unsigned int feature, CState* s, CAction* a);
	void setOffset(double offset);
};

class CActionVariableGrid : public CSingleDimensionGrid
{
	ACTION_VARIABLE m_hVariable;
public:
	void initVarRange();
	CActionVariableGrid(int m_hVar, int numCenters, Distribution distr = Distribution::linear);
	CActionVariableGrid(CConfigNode* pParameters);
	double getVarValue(const CState* s, const CAction* a);
	CNamedVarProperties& getVarProperties(const CState* s, const CAction* a);
	void setFeatureStateAction(unsigned int feature, CState* s, CAction* a);
	void setOffset(double offset);
};