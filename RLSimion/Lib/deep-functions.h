#pragma once

#include "parameters.h"
#include "parameters-numeric.h"
#include "deep-layer.h"
#include "deep-learner.h"
#include "../Common/state-action-function.h"

class IDeepNetwork;
class DeepMinibatch;

class DeepNetworkDefinition
{
public:
	static const char layerDefinitionDelimiter = ';';
	static const char layerParameterDelimiter = ',';
	static const char learnerParameterDelimiter = ',';
	
protected:
	MULTI_VALUE<DeepLayer> m_layers;
	CHILD_OBJECT_FACTORY<DeepLearner> m_learner;
	BOOL_PARAM m_useMinibatchNormalization;
	INT_PARAM m_minibatchSize;

	size_t m_numOutputs = 0;
	vector<string> m_inputStateVariables;
	vector<string> m_inputActionVariables;

	DeepNetworkDefinition() {}
public:
	DeepNetworkDefinition(ConfigNode* pConfigNode);

	unsigned int getNumOutputs() { return (int)m_numOutputs; }
	virtual const vector<string>& getInputStateVariables() { return m_inputStateVariables; }
	virtual const vector<string>& getInputActionVariables() { return m_inputActionVariables; }
	virtual const vector<string>& getUsedActionVariables() { return m_inputActionVariables; }

	string getLayersDefinition();
	string getLearnerDefinition();
	bool useNormalization();

	void stateToVector(const State* s, vector<double>& v, size_t numTuples);
	void actionToVector(const Action* s, vector<double>& v, size_t numTuples);
	void vectorToState(vector<double>& v, size_t numTuples, State* s);
	void vectorToAction(vector<double>& v, size_t numTuples, Action* s);

	DeepMinibatch* getMinibatch();
	double getLearningRate();
};

class DeepDiscreteQFunction: public DeepNetworkDefinition
{
	MULTI_VALUE_VARIABLE<STATE_VARIABLE> m_inputState;
	vector <string> m_outputActionVariables;
	MULTI_VALUE_VARIABLE<ACTION_VARIABLE> m_outputAction;
	INT_PARAM m_numActionSteps;
	int m_totalNumActionSteps = 0;
public:
	DeepDiscreteQFunction();
	DeepDiscreteQFunction(ConfigNode* pConfigNode);

	size_t getNumStepsPerAction() const;
	size_t getTotalNumActionSteps() const;
	size_t getNumOutputActions() const;
	vector<string>& getOutputActionVariables();
	virtual const vector<string>& getUsedActionVariables() { return m_outputActionVariables; }

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
	vector <string> m_outputActionVariables;
public:
	DeepDeterministicPolicy();
	DeepDeterministicPolicy(ConfigNode* pConfigNode);

	IDeterministicPolicyNetwork* getNetworkInstance();
	vector<string>& getOutputActionVariables();
	virtual const vector<string>& getUsedActionVariables() { return m_outputActionVariables; }
};
