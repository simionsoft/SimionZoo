#pragma once

#include <vector>
using namespace std;

#include "parameters-numeric.h"

class Noise;

class NamedVarSet;
using State = NamedVarSet;
using Action = NamedVarSet;

class INetwork;

class DiscreteDeepPolicy
{
protected:
	//We store in this vector a vector the discretized values for each output action
	vector<double> m_discretizedAction;
	vector<double> m_argMaxAction; //only declared to use in evaluation of Q(s,a)
	vector<double> m_stateVector;
	vector<double> m_argMaxQValues;

	vector<string> m_outputActionVariables;

	int m_numTotalActionSamples = 0;
	int m_numSamplesPerActionVariable = 0;
	int m_numActionVariables = 0;

	void randomActionSelection(INetwork* pNetwork, const State* s, Action* a);
	void greedyActionSelection(INetwork* pNetwork, const State* s, Action* a);

	size_t getActionVariableIndex(double value);
public:
	static std::shared_ptr<DiscreteDeepPolicy> getInstance(ConfigNode* pConfigNode);

	DiscreteDeepPolicy(ConfigNode* pConfigNode);

	void initialize(MULTI_VALUE<STATE_VARIABLE>& inputState, MULTI_VALUE<ACTION_VARIABLE>& outputActions, int numSamplesPerActionVariable);

	size_t getActionIndex(const vector<double>& action, int actionOffset);

	virtual void selectAction(INetwork* pNetwork, const State* s, Action* a) = 0;
};

class DiscreteEpsilonGreedyDeepPolicy : public DiscreteDeepPolicy
{
protected:
	CHILD_OBJECT_FACTORY<NumericValue> m_epsilon;
public:
	DiscreteEpsilonGreedyDeepPolicy(ConfigNode* pConfigNode);

	virtual void selectAction(INetwork* pNetwork, const State* s, Action* a);
};

class DiscreteSoftmaxDeepPolicy : public DiscreteDeepPolicy
{
protected:
	CHILD_OBJECT_FACTORY<NumericValue> m_temperature;
public:
	DiscreteSoftmaxDeepPolicy(ConfigNode* pConfigNode);

	virtual void selectAction(INetwork* pNetwork, const State* s, Action* a);
};

class DiscreteExplorationDeepPolicy : public DiscreteDeepPolicy
{
protected:
	int m_lastAction = 0;
	double m_epsilon = 0.98;
public:
	DiscreteExplorationDeepPolicy(ConfigNode* pConfigNode);

	virtual void selectAction(INetwork* pNetwork, const State* s, Action* a);
};

class NoisePlusGreedyDeepPolicy : public DiscreteDeepPolicy
{
	MULTI_VALUE_FACTORY<Noise> m_noiseSignals;
public:
	NoisePlusGreedyDeepPolicy(ConfigNode* pConfigNode);

	virtual void selectAction(INetwork* pNetwork, const State* s, Action* a);
};