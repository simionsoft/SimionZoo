#pragma once

#include "parameters.h"
#include "parameters-numeric.h"
#include "deep-layer.h"
#include "deferred-load.h"

class DeepFunction: public DeferredLoad
{
	MULTI_VALUE<DeepLayer> m_layers;
	ENUM_PARAM<DeepLearner> m_learner;
	BOOL_PARAM m_useMinibatchNormalization;
	INT_PARAM m_minibatchSize;
	CHILD_OBJECT_FACTORY<NumericValue> m_learningRate;
protected:
	DeepFunction() {}
public:
	DeepFunction(ConfigNode* pConfigNode);

	virtual void deferredLoadStep() = 0;
};

class DeepDiscreteQFunction: public DeepFunction
{
	MULTI_VALUE_VARIABLE<STATE_VARIABLE> m_inputState;
	MULTI_VALUE_VARIABLE<ACTION_VARIABLE> m_outputAction;
	INT_PARAM m_numActionSteps;
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
