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
class CStateFeatureMap: public CParamObject
{
public:
	CStateFeatureMap(CParameters* pParameters);
	virtual void getFeatures(const CState* s,CFeatureList* outFeatures)= 0;
	virtual void getFeatureState(unsigned int feature, CState* s)= 0;

	virtual unsigned int getTotalNumFeatures()= 0;
	virtual unsigned int getMaxNumActiveFeatures()= 0;

	static CStateFeatureMap* getInstance(CParameters* pParameters);
};

class CActionFeatureMap : public CParamObject
{
public:
	CActionFeatureMap(CParameters* pParameters);
	virtual void getFeatures(const CAction* a, CFeatureList* outFeatures) = 0;
	virtual void getFeatureAction(unsigned int feature, CAction* a) = 0;

	virtual unsigned int getTotalNumFeatures() = 0;
	virtual unsigned int getMaxNumActiveFeatures() = 0;

	static CActionFeatureMap* getInstance(CParameters* pParameters);
};


//CFeatureMap////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
#define MAX_NUM_ACTIVE_FEATURES_PER_DIMENSION 3


class CGaussianRBFGridFeatureMap
{
protected:
	CFeatureList* m_pVarFeatures;

	unsigned int m_totalNumFeatures;
	unsigned int m_maxNumActiveFeatures;

	int m_numDimensions;
	CSingleDimensionGrid **m_pGrid;

	CGaussianRBFGridFeatureMap(CParameters* pParameters);
public:

	virtual ~CGaussianRBFGridFeatureMap();

	void getFeatures(const CState* s,const CAction* a,CFeatureList* outFeatures);
	void getFeatureStateAction(unsigned int feature, CState* s, CAction* a);

};

class CGaussianRBFStateGridFeatureMap : public CGaussianRBFGridFeatureMap, public CStateFeatureMap
{
public:
	CGaussianRBFStateGridFeatureMap(CParameters* pParameters);
	void getFeatures(const CState* s, CFeatureList* outFeatures){ CGaussianRBFGridFeatureMap::getFeatures(s, 0, outFeatures); }
	void getFeatureState(unsigned int feature, CState* s){ CGaussianRBFGridFeatureMap::getFeatureStateAction(feature, s, 0); }
	unsigned int getTotalNumFeatures(){ return m_totalNumFeatures; }
	unsigned int getMaxNumActiveFeatures(){ return m_maxNumActiveFeatures; }

};

class CGaussianRBFActionGridFeatureMap : public CGaussianRBFGridFeatureMap, public CActionFeatureMap
{
public:
	CGaussianRBFActionGridFeatureMap(CParameters* pParameters);
	void getFeatures(const CAction* a, CFeatureList* outFeatures){ CGaussianRBFGridFeatureMap::getFeatures(0, a, outFeatures); }
	void getFeatureAction(unsigned int feature, CAction* a){ CGaussianRBFGridFeatureMap::getFeatureStateAction(feature, 0, a); }
	unsigned int getTotalNumFeatures(){ return m_totalNumFeatures; }
	unsigned int getMaxNumActiveFeatures(){ return m_maxNumActiveFeatures; }

};