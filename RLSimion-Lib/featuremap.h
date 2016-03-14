#pragma once

class CFeatureList;
class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;
class CParameters;
class CSingleDimensionGrid;
#include "parameterized-object.h"

//enum VariableType{StateVariable,ActionVariable};

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
#define MAX_NUM_ACTIVE_FEATURES_PER_DIMENSION 3


class CGaussianRBFGridFeatureMap: public CFeatureMap, public CParamObject
{
protected:
	CFeatureList* m_pVarFeatures;

	unsigned int m_totalNumFeatures;
	unsigned int m_maxNumActiveFeatures;

	int m_numDimensions;
	CSingleDimensionGrid **m_pGrid;

	CGaussianRBFGridFeatureMap(CParameters* pParameters);
public:

	~CGaussianRBFGridFeatureMap();

	void getFeatures(const CState* s,const CAction* a,CFeatureList* outFeatures);
	void getFeatureStateAction(unsigned int feature, CState* s, CAction* a);

	unsigned int getTotalNumFeatures(){return m_totalNumFeatures;}
	unsigned int getMaxNumActiveFeatures(){return m_maxNumActiveFeatures;}
};

class CGaussianRBFStateGridFeatureMap : public CGaussianRBFGridFeatureMap
{
public:
	CGaussianRBFStateGridFeatureMap(CParameters* pParameters);
};

class CGaussianRBFActionGridFeatureMap : public CGaussianRBFGridFeatureMap
{
public:
	CGaussianRBFActionGridFeatureMap(CParameters* pParameters);
};