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
class CPolicy : public CParamObject
{
protected:
	int m_outputActionIndex;
	//const char* m_outputAction;
public:
	CPolicy(CParameters* pParameters);
	virtual ~CPolicy();

	virtual void getFeatures(const CState* state, CFeatureList* outFeatureList)= 0;
	virtual void addFeatures(const CFeatureList* pFeatureList, double factor) = 0;
	virtual double getDeterministicOutput(const CFeatureList* pFeatureList) = 0;

	virtual void selectAction(const CState *s, CAction *a) = 0;

	virtual void getNaturalGradient(const CState* s, const CAction* a, CFeatureList* pOutGradient) = 0;

//	const char* getOutputAction(){ return m_outputAction; }
	int getOutputActionIndex(){ return m_outputActionIndex; }

	static CPolicy* getInstance(CParameters* pParameters);
};

//A policy that adds noise drawn from N(VFA(s),sigma) deterministic
//As proposed by Hasselt (?)

class CDeterministicPolicyGaussianNoise : public CPolicy
{
protected:
	CLinearStateVFA *m_pDeterministicVFA;
	CNoise *m_pExpNoise;

public:
	CDeterministicPolicyGaussianNoise(CParameters* pParameters);
	~CDeterministicPolicyGaussianNoise();

	void getFeatures(const CState* state, CFeatureList* outFeatureList);
	void addFeatures(const CFeatureList* pFeatureList, double factor);
	double getDeterministicOutput(const CFeatureList* pFeatureList);

	void selectAction(const CState *s, CAction *a);

	void getNaturalGradient(const CState* s, const CAction* a, CFeatureList* pOutGradient);
};

//A policy that adds noise drawn from N(VFA(s),sigma) deterministic
//Model-Free Reinforcement Learning with Continuous Action in Practice
//2012 American Control Conference

class CStochasticPolicyGaussianNoise : public CPolicy
{
protected:
	//The deterministic output. The indices of the weights start from 0
	CLinearStateVFA *m_pMeanVFA;
	//the sigma parameter of the Gaussian noise added to the deterministic output. The indices of the VFA's start from m_pMeanVFA->getNumWeigths()
	CLinearStateVFA *m_pSigmaVFA;
	//Auxiliar feature lists
	CFeatureList *m_pMeanFeatures,*m_pSigmaFeatures;
public:
	CStochasticPolicyGaussianNoise(CParameters* pParameters);
	~CStochasticPolicyGaussianNoise();

	void getFeatures(const CState* state, CFeatureList* outFeatureList);
	void addFeatures(const CFeatureList* pFeatureList, double factor);
	double getDeterministicOutput(const CFeatureList* pFeatureList);

	void selectAction(const CState *s, CAction *a);

	void getNaturalGradient(const CState* s, const CAction* a, CFeatureList* pOutGradient);
};