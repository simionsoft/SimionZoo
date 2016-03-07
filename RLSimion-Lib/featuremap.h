#pragma once

class CFeatureList;
class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;
class CParameters;
#include "parameterized-object.h"

enum MapType{StateMap,ActionMap};

//CFeatureMap//////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
class CFeatureMap
{
public:
	virtual void getFeatures(const CState* s,const CAction* a,CFeatureList* outFeatures)= 0;
	virtual void getFeatureStateAction(unsigned int feature, CState* s, CAction* a)= 0;

	virtual unsigned int getTotalNumFeatures()= 0;
	virtual unsigned int getMaxNumActiveFeatures()= 0;

	static CFeatureMap* getInstance(CParameters* pParameters);
};


//CFeatureMap////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
#define STATE_VARIABLE 0
#define ACTION_VARIABLE 1

#define MAX_NUM_ACTIVE_FEATURES_PER_DIMENSION 3

class CGaussianRBFGridFeatureMap: public CFeatureMap, public CParamObject
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

	void initCenterPoints(int i,const char* varName,int numCenters
					  ,double minV, double maxV,const char* distribution);

	double getDimValue(int dim, const CState* s, const CAction* a);
	void getDimFeatures(int dim, double value, CFeatureList* outDimFeatures);
	double getFeatureFactor(int dim, int feature,double value);
public:
	CGaussianRBFGridFeatureMap(CParameters* pParameters);
	~CGaussianRBFGridFeatureMap();

	void getFeatures(const CState* s,const CAction* a,CFeatureList* outFeatures);
	void getFeatureStateAction(unsigned int feature, CState* s, CAction* a);

	unsigned int getTotalNumFeatures(){return m_totalNumFeatures;}
	unsigned int getMaxNumActiveFeatures(){return m_maxNumActiveFeatures;}
};

