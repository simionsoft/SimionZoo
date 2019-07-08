#pragma once

#include "parameters.h"
#include "parameters-numeric.h"
#include "deep-layer.h"
#include "deferred-load.h"
#include "../Common/state-action-function.h"

class DeepFunction: public DeferredLoad, StateActionFunction
{
	MULTI_VALUE<DeepLayer> m_layers;
	ENUM_PARAM<DeepLearner> m_learner;
	BOOL_PARAM m_useMinibatchNormalization;
	INT_PARAM m_minibatchSize;
	CHILD_OBJECT_FACTORY<NumericValue> m_learningRate;
protected:
	int m_numOutputs = 0;
	vector<string> m_inputStateVariables;
	vector<string> m_inputActionVariables;
	DeepFunction() {}
public:
	DeepFunction(ConfigNode* pConfigNode);

	//DeferredLoad
	virtual void deferredLoadStep() = 0;

	//StateActionFunction
	//virtual unsigned int getNumOutputs() = 0;
	//virtual vector<double>& evaluate(const State* s, const Action* a) = 0;
	virtual const vector<string>& getInputStateVariables() { return m_inputStateVariables; }
	virtual const vector<string>& getInputActionVariables() { return m_inputActionVariables; }
};

class DeepDiscreteQFunction: public DeepFunction
{
	MULTI_VALUE_VARIABLE<STATE_VARIABLE> m_inputState;
	MULTI_VALUE_VARIABLE<ACTION_VARIABLE> m_outputAction;
	INT_PARAM m_numActionSteps;
	int m_totalNumActionSteps = 0;
public:
	DeepDiscreteQFunction();
	DeepDiscreteQFunction(ConfigNode* pConfigNode);

	void deferredLoadStep();
};

class DeepContinuousQFunction : public DeepFunction
{
	MULTI_VALUE_VARIABLE<STATE_VARIABLE> m_inputState;
	MULTI_VALUE_VARIABLE<ACTION_VARIABLE> m_inputAction;
public:
	DeepContinuousQFunction();
	DeepContinuousQFunction(ConfigNode* pConfigNode);

	void deferredLoadStep();
};

class DeepVFunction : public DeepFunction
{
	MULTI_VALUE_VARIABLE<STATE_VARIABLE> m_inputState;
public:
	DeepVFunction();
	DeepVFunction(ConfigNode* pConfigNode);
	void deferredLoadStep();
};

class DeepDeterministicPolicy : public DeepFunction
{
	MULTI_VALUE_VARIABLE<STATE_VARIABLE> m_inputState;
	MULTI_VALUE_VARIABLE<ACTION_VARIABLE> m_outputAction;
public:
	DeepDeterministicPolicy();
	DeepDeterministicPolicy(ConfigNode* pConfigNode);
	void deferredLoadStep();
};
