#pragma once
class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;
class CFeatureList;
class CConfigNode;
#include "parameters.h"

#define ACTIVATION_THRESHOLD 0.0001

class CSingleDimensionGrid
{
protected:
	INT_PARAM m_numCenters;
	double *m_pCenters;

	double m_min;
	double m_max;
	ENUM_PARAM<Distribution> m_distributionType;

	CSingleDimensionGrid();

public:
	virtual ~CSingleDimensionGrid();

	void initCenterPoints();

	int getNumCenters(){ return m_numCenters.get(); }
	double* getCenters(){ return m_pCenters; }

	void getFeatures(const CState* s, const CAction* a, CFeatureList* outDimFeatures);
	double getFeatureFactor(int feature, double value);

	virtual double getVariableValue(const CState* s, const CAction* a) = 0;
	virtual void setFeatureStateAction(unsigned int feature, CState* s, CAction* a) = 0;
};

class CStateVariableGrid : public CSingleDimensionGrid
{
	STATE_VARIABLE m_hVariable;
public:
	CStateVariableGrid(CConfigNode* pParameters);
	double getVariableValue(const CState* s, const CAction* a);
	void setFeatureStateAction(unsigned int feature, CState* s, CAction* a);
};

class CActionVariableGrid : public CSingleDimensionGrid
{
	ACTION_VARIABLE m_hVariable;
public:
	CActionVariableGrid(CConfigNode* pParameters);
	double getVariableValue(const CState* s, const CAction* a);
	void setFeatureStateAction(unsigned int feature, CState* s, CAction* a);
};