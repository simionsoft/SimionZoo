#pragma once
class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;
class CFeatureList;
class CConfigNode;

class CSingleDimensionGrid
{
protected:
	int m_numCenters;
	double *m_pCenters;
	//VariableType m_variableType;
	int m_variableIndex;
	double m_min;
	double m_max;
	const char* m_distributionType;

	CSingleDimensionGrid();

public:
	virtual ~CSingleDimensionGrid();

	void initCenterPoints();

	int getNumCenters(){ return m_numCenters; }
	double* getCenters(){ return m_pCenters; }

	void getFeatures(const CState* s, const CAction* a, CFeatureList* outDimFeatures);
	double getFeatureFactor(int feature, double value);

	virtual double getVariableValue(const CState* s, const CAction* a) = 0;
	virtual void setFeatureStateAction(unsigned int feature, CState* s, CAction* a) = 0;
};

class CStateVariableGrid : public CSingleDimensionGrid
{
public:
	CStateVariableGrid(CConfigNode* pParameters);
	double getVariableValue(const CState* s, const CAction* a);
	void setFeatureStateAction(unsigned int feature, CState* s, CAction* a);
};

class CActionVariableGrid : public CSingleDimensionGrid
{
public:
	CActionVariableGrid(CConfigNode* pParameters);
	double getVariableValue(const CState* s, const CAction* a);
	void setFeatureStateAction(unsigned int feature, CState* s, CAction* a);
};