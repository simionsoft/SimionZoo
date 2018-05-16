#pragma once
#include "parameters.h"
class LinearStateVFA;
class Noise;

class NamedVarSet;
typedef NamedVarSet State;
typedef NamedVarSet Action;

class ConfigNode;
class FeatureList;


//The interface class. One State-Function that represents a deterministic function output to some action
class Policy
{
protected:
	ACTION_VARIABLE m_outputAction;
public:
	Policy(ConfigNode* pConfigNode);
	virtual ~Policy();

	virtual void getFeatures(const State* state, FeatureList* outFeatureList) = 0;
	virtual void addFeatures(const FeatureList* pFeatureList, double factor) = 0;
	virtual double getDeterministicOutput(const FeatureList* pFeatureList) = 0;

	//this method is used when we want the policy to plug directly its output into the environment
	virtual double selectAction(const State *s, Action *a) = 0;

	//getProbability returns the probability with which the policy would select this output in s
	//if we want to take exploration into account, bStochastic should be true. False otherwise.
	virtual double getProbability(const State *s, const Action *a, bool bStochastic) = 0;

	virtual void getParameterGradient(const State* s, const Action* a, FeatureList* pOutGradient) = 0;

	size_t getOutputActionIndex() { return m_outputAction.get(); }
	void setOutputActionIndex(size_t outputActionIndex) { m_outputAction.set(outputActionIndex); }

	static std::shared_ptr<Policy> getInstance(ConfigNode* pParameters);

	virtual LinearStateVFA* getDetPolicyStateVFA() = 0;
};

class DeterministicPolicy : public Policy
{
public:
	DeterministicPolicy(ConfigNode* pConfigNode);

	static std::shared_ptr<DeterministicPolicy> getInstance(ConfigNode* pParameters) {throw("Not implemented: DeterministicPolicy::getInstance()");}
};

class StochasticPolicy : public Policy
{
public:
	StochasticPolicy(ConfigNode* pConfigNode);
	static std::shared_ptr<StochasticPolicy> getInstance(ConfigNode* pParameters) { throw("Not implemented: StochasticPolicy::getInstance()"); }
};


//A policy that adds noise drawn from N(VFA(s),sigma) deterministic
//As proposed by Hasselt (?)
class DeterministicPolicyGaussianNoise : public DeterministicPolicy
{
	double m_lastNoise;
protected:
	CHILD_OBJECT<LinearStateVFA> m_pDeterministicVFA;
	CHILD_OBJECT_FACTORY<Noise> m_pExpNoise;

public:
	DeterministicPolicyGaussianNoise(ConfigNode* pParameters);
	virtual ~DeterministicPolicyGaussianNoise();

	void getFeatures(const State* state, FeatureList* outFeatureList);
	void addFeatures(const FeatureList* pFeatureList, double factor);
	double getDeterministicOutput(const FeatureList* pFeatureList);

	double selectAction(const State *s, Action *a);
	double getProbability(const State *s, const Action *a, bool bStochastic);

	void getParameterGradient(const State* s, const Action* a, FeatureList* pOutGradient);

	LinearStateVFA* getDetPolicyStateVFA() { return m_pDeterministicVFA.ptr(); }
};

//A policy that adds noise drawn from N(VFA(s),sigma) deterministic
//Model-Free Reinforcement Learning with Continuous Action in Practice
//2012 American Control Conference
class StochasticGaussianPolicy : public StochasticPolicy
{
	double m_lastNoise;
protected:
	//The deterministic output. The indices of the weights start from 0
	CHILD_OBJECT<LinearStateVFA> m_pMeanVFA;
	//the sigma parameter of the Gaussian noise added to the deterministic output. The indices of the VFA's start from m_pMeanVFA->getNumWeigths()
	CHILD_OBJECT<LinearStateVFA> m_pSigmaVFA;
	//Auxiliar feature lists
	FeatureList *m_pMeanFeatures, *m_pSigmaFeatures;

	//temp buffer lists for the add() method
	//FeatureList* m_pMeanAddList;
	//FeatureList* m_pSigmaAddList;
public:
	StochasticGaussianPolicy(ConfigNode* pParameters);
	virtual ~StochasticGaussianPolicy();

	void getFeatures(const State* state, FeatureList* outFeatureList);
	void addFeatures(const FeatureList* pFeatureList, double factor);
	double getDeterministicOutput(const FeatureList* pFeatureList);

	double selectAction(const State *s, Action *a);
	double getProbability(const State *s, const Action *a, bool bStochastic);

	void getParameterGradient(const State* s, const Action* a, FeatureList* pOutGradient);

	LinearStateVFA* getDetPolicyStateVFA() { return m_pMeanVFA.ptr(); }
};
