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
	//these two vectors are used to store a single state/action
	vector<double> m_stateVector;
	vector<double> m_actionVector;

	vector<double> m_argMaxQ;
	vector<double> m_argMaxStateAction;

	int m_numSamplesPerActionVariable = 0;
	int m_numArgMaxTotalSamples = 0;
	int m_numTuplesInMinibatch = 0;

	void randomActionSelection(INetwork* pNetwork, const State* s, Action* a);
	void greedyActionSelection(INetwork* pNetwork, const State* s, Action* a);
public:
	static std::shared_ptr<DiscreteDeepPolicy> getInstance(ConfigNode* pConfigNode);

	DiscreteDeepPolicy(ConfigNode* pConfigNode);

	void initialize(INetworkDefinition* pDefinition, Descriptor& aDesc, int numSamplesPerActionVariable);

	void argMaxValue(INetwork* pNetwork, const vector<double>& state, vector<double>& outAction, int tupleOffset = 0);
	void maxValue(INetwork* pNetwork, const vector<double>& state, double* pOutMaxQ, int tupleOffset = 0);

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