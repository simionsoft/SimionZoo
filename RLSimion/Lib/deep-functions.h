#pragma once

#include "parameters.h"
#include "parameters-numeric.h"
#include "deep-layer.h"
#include "../Common/state-action-function.h"

class IDeepNetwork;
class DeepMinibatch;

class DeepNetworkDefinition
{
protected:
	MULTI_VALUE<DeepLayer> m_layers;
	ENUM_PARAM<DeepLearner> m_learner;
	BOOL_PARAM m_useMinibatchNormalization;
	INT_PARAM m_minibatchSize;
	CHILD_OBJECT_FACTORY<NumericValue> m_learningRate;

	size_t m_numOutputs = 0;
	vector<string> m_inputStateVariables;
	vector<string> m_inputActionVariables;

	string getLayersString();

	DeepNetworkDefinition() {}
public:
	DeepNetworkDefinition(ConfigNode* pConfigNode);

	unsigned int getNumOutputs() { return (int)m_numOutputs; }
	virtual const vector<string>& getInputStateVariables() { return m_inputStateVariables; }
	virtual const vector<string>& getInputActionVariables() { return m_inputActionVariables; }

	void stateToVector(const State* s, vector<double>& v, size_t numTuples);
	void actionToVector(const Action* s, vector<double>& v, size_t numTuples);

	DeepMinibatch* getMinibatch();
};

class DeepDiscreteQFunction: public DeepNetworkDefinition
{
	MULTI_VALUE_VARIABLE<STATE_VARIABLE> m_inputState;
	MULTI_VALUE_VARIABLE<ACTION_VARIABLE> m_outputAction;
	INT_PARAM m_numActionSteps;
	int m_totalNumActionSteps = 0;
public:
	DeepDiscreteQFunction();
	DeepDiscreteQFunction(ConfigNode* pConfigNode);

	IDiscreteQFunctionNetwork* getNetworkInstance();
};

class DeepContinuousQFunction : public DeepNetworkDefinition
{
	MULTI_VALUE_VARIABLE<STATE_VARIABLE> m_inputState;
	MULTI_VALUE_VARIABLE<ACTION_VARIABLE> m_inputAction;
public:
	DeepContinuousQFunction();
	DeepContinuousQFunction(ConfigNode* pConfigNode);

	IContinuousQFunctionNetwork* getNetworkInstance();
};

class DeepVFunction : public DeepNetworkDefinition
{
	MULTI_VALUE_VARIABLE<STATE_VARIABLE> m_inputState;
public:
	DeepVFunction();
	DeepVFunction(ConfigNode* pConfigNode);

	IVFunctionNetwork* getNetworkInstance();
};

class DeepDeterministicPolicy : public DeepNetworkDefinition
{
	MULTI_VALUE_VARIABLE<STATE_VARIABLE> m_inputState;
	MULTI_VALUE_VARIABLE<ACTION_VARIABLE> m_outputAction;
public:
	DeepDeterministicPolicy();
	DeepDeterministicPolicy(ConfigNode* pConfigNode);

	IDeterministicPolicyNetwork* getNetworkInstance();
};
