#pragma once

class CFeatureList;
class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;
class CConfigNode;

#include "parameters.h"
#include "single-dimension-grid.h"
#include <vector>

//CFeatureMap//////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
class CStateFeatureMap
{
	CConfigNode* m_pParameters;
public:
	CStateFeatureMap(CConfigNode* pParameters);
	virtual ~CStateFeatureMap(){};

	virtual void getFeatures(const CState* s,CFeatureList* outFeatures)= 0;
	virtual void getFeatureState(unsigned int feature, CState* s)= 0;

	virtual unsigned int getTotalNumFeatures()= 0;
	virtual unsigned int getMaxNumActiveFeatures()= 0;

	static std::shared_ptr<CStateFeatureMap> getInstance(CConfigNode* pParameters);

	CConfigNode* getParameters() { return m_pParameters; }
};

class CActionFeatureMap
{
	CConfigNode* m_pParameters;
public:
	CActionFeatureMap(CConfigNode* pParameters);
	virtual ~CActionFeatureMap(){};

	virtual void getFeatures(const CAction* a, CFeatureList* outFeatures) = 0;
	virtual void getFeatureAction(unsigned int feature, CAction* a) = 0;

	virtual unsigned int getTotalNumFeatures() = 0;
	virtual unsigned int getMaxNumActiveFeatures() = 0;

	static std::shared_ptr<CActionFeatureMap> getInstance(CConfigNode* pParameters);
	
	CConfigNode* getParameters() { return m_pParameters; }
};


//CFeatureMap////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
#define MAX_NUM_ACTIVE_FEATURES_PER_DIMENSION 3

template<typename varType>
class CGaussianRBFGridFeatureMap
{
protected:
	CFeatureList* m_pVarFeatures;

	unsigned int m_totalNumFeatures;
	unsigned int m_maxNumActiveFeatures;

	MULTI_VALUE<CSingleDimensionGrid<varType>> m_grid;

	CGaussianRBFGridFeatureMap(CConfigNode* pParameters);
public:

	virtual ~CGaussianRBFGridFeatureMap();

	void getFeatures(const CState* s,const CAction* a,CFeatureList* outFeatures);
	void getFeatureStateAction(unsigned int feature, CState* s, CAction* a);

};

class CGaussianRBFStateGridFeatureMap : public CGaussianRBFGridFeatureMap<STATE_VARIABLE>
	, public CStateFeatureMap
{
public:
	CGaussianRBFStateGridFeatureMap(CConfigNode* pParameters);

	void getFeatures(const CState* s, CFeatureList* outFeatures){ CGaussianRBFGridFeatureMap::getFeatures(s, 0, outFeatures); }
	void getFeatureState(unsigned int feature, CState* s){ CGaussianRBFGridFeatureMap::getFeatureStateAction(feature, s, 0); }
	unsigned int getTotalNumFeatures(){ return m_totalNumFeatures; }
	unsigned int getMaxNumActiveFeatures(){ return m_maxNumActiveFeatures; }

};

class CGaussianRBFActionGridFeatureMap : public CGaussianRBFGridFeatureMap<ACTION_VARIABLE>
	, public CActionFeatureMap
{
public:
	CGaussianRBFActionGridFeatureMap(CConfigNode* pParameters);
	
	void getFeatures(const CAction* a, CFeatureList* outFeatures){ CGaussianRBFGridFeatureMap::getFeatures(0, a, outFeatures); }
	void getFeatureAction(unsigned int feature, CAction* a){ CGaussianRBFGridFeatureMap::getFeatureStateAction(feature, 0, a); }
	unsigned int getTotalNumFeatures(){ return m_totalNumFeatures; }
	unsigned int getMaxNumActiveFeatures(){ return m_maxNumActiveFeatures; }

};