#pragma once
class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;
class CFeatureList;
class CConfigNode;
#include "parameters.h"
#include "named-var-set.h"

#define ACTIVATION_THRESHOLD 0.0001

class CSingleDimensionGridRBF
{
protected:
	INT_PARAM m_numCenters;
	double *m_pCenters;

	double m_min;
	double m_max;
	ENUM_PARAM<Distribution> m_distributionType;

	CSingleDimensionGridRBF();

public:
	virtual ~CSingleDimensionGridRBF();

	virtual void initVarRange()= 0;
	void initCenterPoints();

	int getNumCenters(){ return m_numCenters.get(); }
	double* getCenters(){ return m_pCenters; }

	void getFeatures(const CState* s, const CAction* a, CFeatureList* outDimFeatures);
	double getFeatureFactor(int feature, double value);

	virtual double getVarValue(const CState* s, const CAction* a) = 0;
	virtual CNamedVarProperties& getVarProperties(const CState* s, const CAction* a) = 0;
	virtual void setFeatureStateAction(unsigned int feature, CState* s, CAction* a) = 0;
};

class CStateVariableGridRBF : public CSingleDimensionGridRBF
{
	STATE_VARIABLE m_hVariable;
public:
	void initVarRange();
	CStateVariableGridRBF(int m_hVar, int numCenters, Distribution distr= Distribution::linear);
	CStateVariableGridRBF(CConfigNode* pParameters);
	double getVarValue(const CState* s, const CAction* a);
	CNamedVarProperties& getVarProperties(const CState* s, const CAction* a);
	void setFeatureStateAction(unsigned int feature, CState* s, CAction* a);
};

class CActionVariableGridRBF : public CSingleDimensionGridRBF
{
	ACTION_VARIABLE m_hVariable;
public:
	void initVarRange();
	CActionVariableGridRBF(int m_hVar, int numCenters, Distribution distr= Distribution::linear);
	CActionVariableGridRBF(CConfigNode* pParameters);
	double getVarValue(const CState* s, const CAction* a);
	CNamedVarProperties& getVarProperties(const CState* s, const CAction* a);
	void setFeatureStateAction(unsigned int feature, CState* s, CAction* a);
};