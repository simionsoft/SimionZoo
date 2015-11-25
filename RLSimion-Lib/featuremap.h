#pragma once

class CFeatureList;
class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;

//CFeatureMap//////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
class CFeatureMap
{
public:
	virtual void getFeatures(CState* s,CAction* a,CFeatureList* outFeatures)= 0;
	virtual void getFeatureStateAction(unsigned int feature, CState* s, CAction* a)= 0;

	virtual unsigned int getTotalNumFeatures()= 0;
	virtual unsigned int getMaxNumActiveFeatures()= 0;
};


//CFeatureMap////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
#define STATE_VARIABLE 0
#define ACTION_VARIABLE 1

#define MAX_NUM_ACTIVE_FEATURES_PER_DIMENSION 3

class CGaussianRBFGridFeatureMap: public CFeatureMap
{
	//CFeatureList* m_pBuffer;
	CFeatureList* m_pVarFeatures;

	unsigned int m_totalNumFeatures;
	unsigned int m_maxNumActiveFeatures;

	int m_numVariables;
	short *m_variableType;
	int *m_variableIndex;

	int *m_pNumCenters;
	double **m_pCenters;

	int getNumVariables(const char* configString);
	void initCenterPoints(int i,char* varName,int numCenters
					  ,double minV= 0.0, double maxV= 0.0,char distType='u');

	double getDimValue(int dim, CState* s, CAction* a);
	void getDimFeatures(int dim, double value, CFeatureList* outDimFeatures);
	double getFeatureFactor(int dim, int feature,double value);
public:
	CGaussianRBFGridFeatureMap(const char* configString);
	~CGaussianRBFGridFeatureMap();

	void getFeatures(CState* s,CAction* a,CFeatureList* outFeatures);
	void getFeatureStateAction(unsigned int feature, CState* s, CAction* a);

	unsigned int getTotalNumFeatures(){return m_totalNumFeatures;}
	unsigned int getMaxNumActiveFeatures(){return m_maxNumActiveFeatures;}
};

