#pragma once
#include "parameterized-object.h"

class CLinearStateVFA;
class CNoise;

class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;

class CParameters;
class CFeatureList;


//The interface class. One State-Function that represents a deterministic function output to some action
class CDeterministicPolicy : public CParamObject
{
protected:
	int m_outputActionIndex;
	//const char* m_outputAction;
public:
	CDeterministicPolicy(CParameters* pParameters);
	virtual ~CDeterministicPolicy();

	virtual void getFeatures(const CState* state, CFeatureList* outFeatureList)= 0;
	virtual void addFeatures(const CFeatureList* pFeatureList, double factor) = 0;
	virtual double getValue(const CFeatureList* pFeatureList) = 0;

	virtual void selectAction(const CState *s, CAction *a) = 0;

	virtual void getNaturalGradient(const CState* s, const CAction* a, CFeatureList* pOutGradient) = 0;

//	const char* getOutputAction(){ return m_outputAction; }
	int getOutputActionIndex(){ return m_outputActionIndex; }

	static CDeterministicPolicy* getInstance(CParameters* pParameters);
};

//A policy that adds noise drawn from N(VFA(s),sigma) deterministic
//As proposed by Hasselt (?)

class CDeterministicPolicyGaussianNoise : public CDeterministicPolicy
{
protected:
	CLinearStateVFA *m_pDeterministicVFA;
	CNoise *m_pExpNoise;

public:
	CDeterministicPolicyGaussianNoise(CParameters* pParameters);
	~CDeterministicPolicyGaussianNoise();

	void getFeatures(const CState* state, CFeatureList* outFeatureList);
	void addFeatures(const CFeatureList* pFeatureList, double factor);
	double getValue(const CFeatureList* pFeatureList);

	void selectAction(const CState *s, CAction *a);

	void getNaturalGradient(const CState* s, const CAction* a, CFeatureList* pOutGradient);
};

//A policy that adds noise drawn from N(VFA(s),sigma) deterministic
//Model-Free Reinforcement Learning with Continuous Action in Practice
//2012 American Control Conference
/*
class CStochasticPolicyGaussianNoise : public CDeterministicPolicy
{
protected:
	CLinearStateVFA *m_pMeanVFA;
	CLinearStateVFA *m_pSigmaVFA;
	CFeatureList *m_pAux;
public:
	CStochasticPolicyGaussianNoise(CParameters* pParameters);
	~CStochasticPolicyGaussianNoise();

	void getFeatures(const CState* state, CFeatureList* outFeatureList);
	void addFeatures(const CFeatureList* pFeatureList, double factor);
	double getValue(const CFeatureList* pFeatureList);

	void selectAction(const CState *s, CAction *a);

	void getNaturalGradient(const CState* s, const CAction* a, CFeatureList* pOutGradient);
};*/