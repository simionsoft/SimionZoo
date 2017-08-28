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
	bool m_discreteActionSpace = false;

	double m_space_density;

public:
	CPolicy(CConfigNode* pConfigNode);
	virtual ~CPolicy();

	virtual void getFeatures(const CState* state, CFeatureList* outFeatureList) = 0;
	virtual void addFeatures(const CFeatureList* pFeatureList, double factor) = 0;
	virtual double getDeterministicOutput(const CFeatureList* pFeatureList) = 0;

	//this method is used when we want the policy to plug directly its output into the environment
	virtual double selectAction(const CState *s, CAction *a) = 0;

	//getProbability returns the probability with which the policy would select this output in s
	//if we want to take exploration into account, bStochastic should be true. False otherwise.
	virtual double getProbability(const CState *s, const CAction *a, bool bStochastic) = 0;

	virtual void getParameterGradient(const CState* s, const CAction* a, CFeatureList* pOutGradient) = 0;
	//virtual void getActionGradient(const CState* s, const CAction* a, CFeatureList* pOutGradient) = 0;
	//virtual void getStateGradient(const CState* s, const CAction* a, CFeatureList* pOutGradient) = 0;

	int getOutputActionIndex() { return m_outputActionIndex.get(); }
	void setOutputActionIndex(unsigned int outputActionIndex) { m_outputActionIndex.set(outputActionIndex); }

	static std::shared_ptr<CPolicy> getInstance(CConfigNode* pParameters);

	virtual CLinearStateVFA* getDetPolicyStateVFA() = 0;
};

class CDeterministicPolicy : public CPolicy
{ };

class CStochasticPolicy : public CPolicy
{ };

class CStochasticUniformPolicy : public CStochasticPolicy
{
protected:
	double m_minActionValue;
	double m_maxActionValue;
	double m_action_width;

public:
	CStochasticUniformPolicy(CConfigNode* pParameters);
	virtual ~CStochasticUniformPolicy();

	void getFeatures(const CState* state, CFeatureList* outFeatureList);
	void addFeatures(const CFeatureList* pFeatureList, double factor);
	double getDeterministicOutput(const CFeatureList* pFeatureList);

	double selectAction(const CState *s, CAction *a);
	double getProbability(const CState *s, const CAction *a, bool bStochastic);

	void getParameterGradient(const CState* s, const CAction* a, CFeatureList* pOutGradient);

	CLinearStateVFA* getDetPolicyStateVFA() { throw "CStochasticUniformPolicy::getDetPolicyStateVFA() is not implemented"; }
};

//A policy that adds noise drawn from N(VFA(s),sigma) deterministic
//As proposed by Hasselt (?)
class CDeterministicPolicyGaussianNoise : public CDeterministicPolicy
{
	double m_lastNoise;
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
	double getProbability(const CState *s, const CAction *a, bool bStochastic);

	void getParameterGradient(const CState* s, const CAction* a, CFeatureList* pOutGradient);

	CLinearStateVFA* getDetPolicyStateVFA() { return m_pDeterministicVFA.ptr(); }
};

//A policy that adds noise drawn from N(VFA(s),sigma) deterministic
//Model-Free Reinforcement Learning with Continuous Action in Practice
//2012 American Control Conference
class CStochasticGaussianPolicy : public CStochasticPolicy
{
	double m_lastNoise;
protected:
	//The deterministic output. The indices of the weights start from 0
	CHILD_OBJECT<CLinearStateVFA> m_pMeanVFA;
	//the sigma parameter of the Gaussian noise added to the deterministic output. The indices of the VFA's start from m_pMeanVFA->getNumWeigths()
	CHILD_OBJECT<CLinearStateVFA> m_pSigmaVFA;
	//Auxiliar feature lists
	CFeatureList *m_pMeanFeatures, *m_pSigmaFeatures;

	//temp buffer lists for the add() method
	//CFeatureList* m_pMeanAddList;
	//CFeatureList* m_pSigmaAddList;
public:
	CStochasticGaussianPolicy(CConfigNode* pParameters);
	virtual ~CStochasticGaussianPolicy();

	void getFeatures(const CState* state, CFeatureList* outFeatureList);
	void addFeatures(const CFeatureList* pFeatureList, double factor);
	double getDeterministicOutput(const CFeatureList* pFeatureList);

	double selectAction(const CState *s, CAction *a);
	double getProbability(const CState *s, const CAction *a, bool bStochastic);

	void getParameterGradient(const CState* s, const CAction* a, CFeatureList* pOutGradient);

	CLinearStateVFA* getDetPolicyStateVFA() { return m_pMeanVFA.ptr(); }
};