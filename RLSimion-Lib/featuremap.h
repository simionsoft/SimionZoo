#pragma once

class CFeatureList;
class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;
class CConfigNode;

#include "parameters.h"
#include "single-dimension-grid-rbf.h"
#include "single-dimension-grid.h"
#include "single-dimension-discrete-grid.h"
#include <vector>

//CFeatureMap//////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
class CStateFeatureMap
{
	CConfigNode* m_pParameters;
public:
	CStateFeatureMap(CConfigNode* pParameters);
	virtual ~CStateFeatureMap() {};

	virtual void getFeatures(const CState* s, CFeatureList* outFeatures) = 0;
	virtual void getFeatureState(unsigned int feature, CState* s) = 0;

	virtual unsigned int getTotalNumFeatures() = 0;
	virtual unsigned int getMaxNumActiveFeatures() = 0;

	static std::shared_ptr<CStateFeatureMap> getInstance(CConfigNode* pParameters);

	CConfigNode* getParameters() { return m_pParameters; }
};

class CActionFeatureMap
{
	CConfigNode* m_pParameters;
public:
	CActionFeatureMap(CConfigNode* pParameters);
	virtual ~CActionFeatureMap() {};

	virtual void getFeatures(const CAction* a, CFeatureList* outFeatures) = 0;
	virtual void getFeatureAction(unsigned int feature, CAction* a) = 0;

	virtual unsigned int getTotalNumFeatures() = 0;
	virtual unsigned int getMaxNumActiveFeatures() = 0;

	static std::shared_ptr<CActionFeatureMap> getInstance(CConfigNode* pParameters);

	CConfigNode* getParameters() { return m_pParameters; }
};


//CGaussianRBFFeatureMap////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
#define MAX_NUM_ACTIVE_FEATURES_PER_DIMENSION 3
template <typename dimensionGridType>
class CGaussianRBFGridFeatureMap
{
protected:
	CFeatureList* m_pVarFeatures;

	unsigned int m_totalNumFeatures;
	unsigned int m_maxNumActiveFeatures;

	MULTI_VALUE<dimensionGridType> m_grid;

	CGaussianRBFGridFeatureMap(CConfigNode* pParameters);
public:

	virtual ~CGaussianRBFGridFeatureMap();

	void getFeatures(const CState* s, const CAction* a, CFeatureList* outFeatures);
	void getFeatureStateAction(unsigned int feature, CState* s, CAction* a);

};

class CGaussianRBFStateGridFeatureMap : public CGaussianRBFGridFeatureMap<CStateVariableGridRBF>
	, public CStateFeatureMap
{
public:
	CGaussianRBFStateGridFeatureMap(CConfigNode* pParameters);

	void getFeatures(const CState* s, CFeatureList* outFeatures) { CGaussianRBFGridFeatureMap::getFeatures(s, 0, outFeatures); }
	void getFeatureState(unsigned int feature, CState* s) { CGaussianRBFGridFeatureMap::getFeatureStateAction(feature, s, 0); }
	unsigned int getTotalNumFeatures() { return m_totalNumFeatures; }
	unsigned int getMaxNumActiveFeatures() { return m_maxNumActiveFeatures; }

};

class CGaussianRBFActionGridFeatureMap : public CGaussianRBFGridFeatureMap<CActionVariableGridRBF>
	, public CActionFeatureMap
{
public:
	CGaussianRBFActionGridFeatureMap(CConfigNode* pParameters);

	void getFeatures(const CAction* a, CFeatureList* outFeatures) { CGaussianRBFGridFeatureMap::getFeatures(0, a, outFeatures); }
	void getFeatureAction(unsigned int feature, CAction* a) { CGaussianRBFGridFeatureMap::getFeatureStateAction(feature, 0, a); }
	unsigned int getTotalNumFeatures() { return m_totalNumFeatures; }
	unsigned int getMaxNumActiveFeatures() { return m_maxNumActiveFeatures; }

};

//CTileCodingFeatureMap////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
template <typename dimensionGridType>
class CTileCodingFeatureMap
{
protected:
	CFeatureList* m_pVarFeatures;

	unsigned int m_totalNumFeatures;
	unsigned int m_maxNumActiveFeatures;
	unsigned int m_numTilings;

	//modified states s and actions a which are "moved" by the offset of the layer
	const CState* modified_s;
	const CState* modified_a;


	double m_tilingOffset;

	MULTI_VALUE<dimensionGridType> m_grid;

	CTileCodingFeatureMap(CConfigNode* pParameters);
public:

	virtual ~CTileCodingFeatureMap();

	void getFeatures(const CState* s, const CAction* a, CFeatureList* outFeatures);
	void getFeatureStateAction(unsigned int feature, CState* s, CAction* a);
	virtual unsigned int getNumTilings() = 0;

	MULTI_VALUE<dimensionGridType> returnGrid() { return m_grid; }

};

class CTileCodingStateFeatureMap : public CTileCodingFeatureMap<CSingleDimensionStateVariableGrid>
	, public CStateFeatureMap
{
public:
	CTileCodingStateFeatureMap(CConfigNode* pParameters);

	void getFeatures(const CState* s, CFeatureList* outFeatures) { CTileCodingFeatureMap::getFeatures(s, 0, outFeatures); }
	void getFeatureState(unsigned int feature, CState* s) { CTileCodingFeatureMap::getFeatureStateAction(feature, s, 0); }
	unsigned int getTotalNumFeatures() { return m_totalNumFeatures; }
	unsigned int getMaxNumActiveFeatures() { return m_maxNumActiveFeatures; }
	unsigned int getNumTilings() { return m_numTilings; }
	double getTilingOffset() { return m_tilingOffset; }
};

class CTileCodingActionFeatureMap : public CTileCodingFeatureMap<CSingleDimensionActionVariableGrid>
	, public CActionFeatureMap
{
public:
	CTileCodingActionFeatureMap(CConfigNode* pParameters);

	void getFeatures(const CAction* a, CFeatureList* outFeatures) { CTileCodingFeatureMap::getFeatures(0, a, outFeatures); }
	void getFeatureAction(unsigned int feature, CAction* a) { CTileCodingFeatureMap::getFeatureStateAction(feature, 0, a); }
	unsigned int getTotalNumFeatures() { return m_totalNumFeatures; }
	unsigned int getMaxNumActiveFeatures() { return m_maxNumActiveFeatures; }
	unsigned int getNumTilings() { return m_numTilings; }
	double getTilingOffset() { return m_tilingOffset; }
};

//CDiscreteFeatureMap////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
template <typename dimensionGridType>
class CDiscreteFeatureMap
{
protected:
	CFeatureList* m_pVarFeatures;

	unsigned int m_totalNumFeatures;
	unsigned int m_maxNumActiveFeatures;

	MULTI_VALUE<dimensionGridType> m_grid;

	CDiscreteFeatureMap(CConfigNode* pParameters);
public:
	virtual ~CDiscreteFeatureMap();

	void getFeatures(const CState* s, const CAction* a, CFeatureList* outFeatures);
	void getFeatureStateAction(unsigned int feature, CState* s, CAction* a);

	MULTI_VALUE<dimensionGridType> returnGrid() { return m_grid; }
};

class CDiscreteStateFeatureMap : public CDiscreteFeatureMap<CSingleDimensionDiscreteStateVariableGrid>
	, public CStateFeatureMap
{
public:
	CDiscreteStateFeatureMap(CConfigNode* pParameters);

	void getFeatures(const CState* s, CFeatureList* outFeatures) { CDiscreteFeatureMap::getFeatures(s, 0, outFeatures); }
	void getFeatureState(unsigned int feature, CState* s) { CDiscreteFeatureMap::getFeatureStateAction(feature, s, 0); }
	unsigned int getTotalNumFeatures() { return m_totalNumFeatures; }
	unsigned int getMaxNumActiveFeatures() { return m_maxNumActiveFeatures; }
};

class CDiscreteActionFeatureMap : public CDiscreteFeatureMap<CSingleDimensionDiscreteActionVariableGrid>
	, public CActionFeatureMap
{
public:
	CDiscreteActionFeatureMap(CConfigNode* pParameters);

	void getFeatures(const CAction* a, CFeatureList* outFeatures) { CDiscreteFeatureMap::getFeatures(0, a, outFeatures); }
	void getFeatureAction(unsigned int feature, CAction* a) { CDiscreteFeatureMap::getFeatureStateAction(feature, 0, a); }
	unsigned int getTotalNumFeatures() { return m_totalNumFeatures; }
	unsigned int getMaxNumActiveFeatures() { return m_maxNumActiveFeatures; }
};

//CBagFeatureMap////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
class CBagStateFeatureMap : public CStateFeatureMap
{
	MULTI_VALUE_VARIABLE<STATE_VARIABLE> m_stateVariables;
public:
	CBagStateFeatureMap(CConfigNode* pParameters);

	void getFeatures(const CState* s, CFeatureList* outFeatures);
	void getFeatureState(unsigned int feature, CState* s);
	unsigned int getTotalNumFeatures() { return m_stateVariables.size(); }
	unsigned int getMaxNumActiveFeatures() { return m_stateVariables.size(); }
};

class CBagActionFeatureMap : public CActionFeatureMap
{
	MULTI_VALUE_VARIABLE<ACTION_VARIABLE> m_actionVariables;
public:
	CBagActionFeatureMap(CConfigNode* pParameters);

	void getFeatures(const CAction* a, CFeatureList* outFeatures);
	void getFeatureAction(unsigned int feature, CAction* a);
	unsigned int getTotalNumFeatures() { return m_actionVariables.size(); }
	unsigned int getMaxNumActiveFeatures() { return m_actionVariables.size(); }
};