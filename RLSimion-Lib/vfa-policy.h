#pragma once
#include "parameterized-object.h"

class CLinearStateVFA;
class CNoise;

class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;

class CParameters;


//The interface class. One State-Function that represents a deterministic function output to some action
class CSingleOutputVFAPolicy : public CParamObject
{
protected:
	CLinearStateVFA *m_pVFA;
	int m_outputActionIndex;
	const char* m_outputAction;
public:
	CSingleOutputVFAPolicy(CParameters* pParameters);
	~CSingleOutputVFAPolicy();

	CLinearStateVFA* getVFA(){ return m_pVFA; }
	//CNoise* getExpNoise(){ return m_pExpNoise; }
	const char* getOutputAction(){ return m_outputAction; }
	int getOutputActionIndex(){ return m_outputActionIndex; }

	static CSingleOutputVFAPolicy* getInstance(CParameters* pParameters);


	virtual void selectAction(const CState *s, CAction *a) = 0;
	//returns the factor by which the state features have to be multiplied to get the policy gradient
	virtual void getGradient(const CState* s, const CAction* a, CFeatureList* pOutGradient) = 0;
};

//A policy that adds noise drawn from N(VFA(s),sigma) deterministic
//As proposed by Hasselt (?)
class CDeterministicPolicyGaussianNoise : public CSingleOutputVFAPolicy
{
protected:
	CNoise *m_pExpNoise;

public:
	CDeterministicPolicyGaussianNoise(CParameters* pParameters);
	~CDeterministicPolicyGaussianNoise();

	void selectAction(const CState *s, CAction *a);

	//returns the factor by which the state features have to be multiplied to get the policy gradient
	void getGradient(const CState* s, const CAction* a, CFeatureList* pOutGradient);
};

//A policy that adds noise drawn from N(VFA(s),sigma) deterministic
//Model-Free Reinforcement Learning with Continuous Action in Practice
//2012 American Control Conference
class CStochasticPolicyGaussianNoise : public CSingleOutputVFAPolicy
{
protected:
	CLinearStateVFA *m_pSigmaVFA;
	CFeatureList *m_pAux;
public:
	CStochasticPolicyGaussianNoise(CParameters* pParameters);
	~CStochasticPolicyGaussianNoise();

	void selectAction(const CState *s, CAction *a);

	//returns the factor by which the state features have to be multiplied to get the policy gradient
	void getGradient(const CState* s, const CAction* a, CFeatureList* pOutGradient);
};