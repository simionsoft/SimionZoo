#pragma once
#include "parameters.h"
class CLinearStateVFA;
class CNoise;

class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;

class CConfigNode;
class CFeatureList;


//The interface class. One State-Function that represents a deterministic function output to some action
class CPolicy 
{
protected:
	ACTION_VARIABLE m_outputActionIndex;

public:
	CPolicy(CConfigNode* pConfigNode);
	virtual ~CPolicy();

	virtual void getFeatures(const CState* state, CFeatureList* outFeatureList)= 0;
	virtual void addFeatures(const CFeatureList* pFeatureList, double factor) = 0;
	virtual double getDeterministicOutput(const CFeatureList* pFeatureList) = 0;

	//this method is used when we want the policy to plug directly its output into the environment
	virtual double selectAction(const CState *s, CAction *a) = 0;
	//this method is used when we want to getSample the output of the controller (exploration noise too) and use for some internal stuff
	//i.e., use the output of the policy learner within an adaptive controller: CExtendedWindTurbine...
	virtual double getOutput(const CState *s) = 0;
	//getProbability returns the probability with which the policy would select this output in s
	//if we want to take exploration into account, bStochastic should be true. False otherwise.
	virtual double getProbability(const CState *s, const CAction *a, bool bStochastic)= 0;

	virtual void getNaturalGradient(const CState* s, const CAction* a, CFeatureList* pOutGradient) = 0;

	int getOutputActionIndex(){ return m_outputActionIndex.get(); }

	static std::shared_ptr<CPolicy> getInstance(CConfigNode* pParameters);

	virtual CLinearStateVFA* getDetPolicyStateVFA()= 0;
};

//A policy that adds noise drawn from N(VFA(s),sigma) deterministic
//As proposed by Hasselt (?)

class CDeterministicPolicyGaussianNoise : public CPolicy
{
protected:
	CHILD_OBJECT<CLinearStateVFA> m_pDeterministicVFA;
	CHILD_OBJECT_FACTORY<CNoise> m_pExpNoise;

public:
	CDeterministicPolicyGaussianNoise(CConfigNode* pParameters);
	virtual ~CDeterministicPolicyGaussianNoise();

	void getFeatures(const CState* state, CFeatureList* outFeatureList);
	void addFeatures(const CFeatureList* pFeatureList, double factor);
	double getDeterministicOutput(const CFeatureList* pFeatureList);

	double selectAction(const CState *s, CAction *a);
	double getOutput(const CState *s);
	double getProbability(const CState *s, const CAction *a, bool bStochastic);

	void getNaturalGradient(const CState* s, const CAction* a, CFeatureList* pOutGradient);

	CLinearStateVFA* getDetPolicyStateVFA(){ return m_pDeterministicVFA.ptr(); }
};

//A policy that adds noise drawn from N(VFA(s),sigma) deterministic
//Model-Free Reinforcement Learning with Continuous Action in Practice
//2012 American Control Conference

class CStochasticPolicyGaussianNoise : public CPolicy
{
protected:
	//The deterministic output. The indices of the weights start from 0
	CHILD_OBJECT<CLinearStateVFA> m_pMeanVFA;
	//the sigma parameter of the Gaussian noise added to the deterministic output. The indices of the VFA's start from m_pMeanVFA->getNumWeigths()
	CHILD_OBJECT<CLinearStateVFA> m_pSigmaVFA;
	//Auxiliar feature lists
	CFeatureList *m_pMeanFeatures,*m_pSigmaFeatures;
public:
	CStochasticPolicyGaussianNoise(CConfigNode* pParameters);
	virtual ~CStochasticPolicyGaussianNoise();

	void getFeatures(const CState* state, CFeatureList* outFeatureList);
	void addFeatures(const CFeatureList* pFeatureList, double factor);
	double getDeterministicOutput(const CFeatureList* pFeatureList);

	double selectAction(const CState *s, CAction *a);
	double getOutput(const CState *s);
	double getProbability(const CState *s, const CAction *a, bool bStochastic);

	void getNaturalGradient(const CState* s, const CAction* a, CFeatureList* pOutGradient);

	CLinearStateVFA* getDetPolicyStateVFA(){ return m_pMeanVFA.ptr(); }
};