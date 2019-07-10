#include "deep-functions.h"
#include "../Common/named-var-set.h"
#include "deep-minibatch.h"
#include "CNTKWrapperClient.h"

DeepNetworkDefinition::DeepNetworkDefinition(ConfigNode* pConfigNode)
{
	m_layers= MULTI_VALUE<DeepLayer>(pConfigNode, "Layers", "Layers of the Neural Network");
	m_learner = CHILD_OBJECT_FACTORY<DeepLearner>(pConfigNode, "Learner", "Learner used for this Neural Network");
	m_useMinibatchNormalization = BOOL_PARAM(pConfigNode, "Use-Normalization", "Use minibatch normalization", false);
	m_minibatchSize = INT_PARAM(pConfigNode, "Minibatch-Size", "Number of tuples in each minibatch used in updates", 100);
	m_learningRate = CHILD_OBJECT_FACTORY<NumericValue>(pConfigNode, "Learning-Rate", "Learning rate used to train the Neural Network");
}

void DeepNetworkDefinition::stateToVector(const State* s, vector<double>& v, size_t numTuples)
{
	const vector<string>& stateVars = getInputStateVariables();
	size_t numStateVars = stateVars.size();

	for (size_t i = 0; i < numStateVars; i++)
		v[numStateVars*numTuples + i] = s->getNormalized(stateVars[i].c_str());
}

void DeepNetworkDefinition::actionToVector(const Action* a, vector<double>& v, size_t numTuples)
{
	const vector<string>& actionVars = getInputActionVariables();
	size_t numActionVars = actionVars.size();

	for (size_t i = 0; i < numActionVars; i++)
		v[numActionVars*numTuples + i] = a->getNormalized(actionVars[i].c_str());
}

void DeepNetworkDefinition::vectorToState(vector<double>& v, size_t numTuples, State* s)
{
	const vector<string>& stateVars = getInputStateVariables();
	size_t numStateVars = stateVars.size();

	for (size_t i = 0; i < numStateVars; i++)
		s->setNormalized(stateVars[i].c_str(), v[numStateVars*numTuples + i]);
}

void DeepNetworkDefinition::vectorToAction(vector<double>& v, size_t numTuples, Action* s)
{
	const vector<string>& actionVars = getInputActionVariables();
	size_t numActionVars = actionVars.size();

	for (size_t i = 0; i < numActionVars; i++)
		s->setNormalized(actionVars[i].c_str(), v[numActionVars*numTuples + i]);
}

string DeepNetworkDefinition::getLayersDefinition()
{
	if (m_layers.size() == 0) return string("");

	string output = m_layers[0]->to_string();

	for (int i = 1; i < m_layers.size(); i++)
		output = output + layerDefinitionDelimiter + m_layers[i]->to_string();

	return output;
}

string DeepNetworkDefinition::getLearnerDefinition()
{
	return m_learner->to_string();
}

DeepMinibatch* DeepNetworkDefinition::getMinibatch()
{
	return new DeepMinibatch(m_minibatchSize.get(), this);
}


DeepDiscreteQFunction::DeepDiscreteQFunction() {}
DeepDiscreteQFunction::DeepDiscreteQFunction(ConfigNode* pConfigNode) : DeepNetworkDefinition(pConfigNode)
{
	m_inputState= MULTI_VALUE_VARIABLE<STATE_VARIABLE>(pConfigNode, "Input-State", "State variables used as input of the Neural Network");
	m_outputAction= MULTI_VALUE_VARIABLE<ACTION_VARIABLE>(pConfigNode, "Output-Action", "Action variables used as output of the Neural Network");
	m_numActionSteps= INT_PARAM(pConfigNode, "Num-Action-Steps", "Number of steps used to discretize each of the output action variables", 100);

	//add state variables to the list stored by the base-class DeepNetworkDefinition
	for (size_t i = 0; i < m_inputState.size(); i++)
		m_inputStateVariables.push_back(m_inputState[i]->get());

	//calculate the total number of discretized actions and set it
	m_totalNumActionSteps = 1;
	for (size_t i = 0; i < m_outputAction.size(); i++)
		m_totalNumActionSteps *= m_numActionSteps.get();

	//One output for each discretized action: [Q(s,a_0), Q(s,a_1),... , Q(s,a_n-1)]
	m_numOutputs = m_totalNumActionSteps;
}

IDiscreteQFunctionNetwork* DeepDiscreteQFunction::getNetworkInstance()
{
	return CNTK::WrapperClient::getDiscreteQFunctionNetwork(m_inputStateVariables.size(), m_totalNumActionSteps
		, getLayersDefinition(), getLearnerDefinition(), m_useMinibatchNormalization.get());
}

DeepContinuousQFunction::DeepContinuousQFunction() {}
DeepContinuousQFunction::DeepContinuousQFunction(ConfigNode* pConfigNode) : DeepNetworkDefinition(pConfigNode)
{
	m_inputState = MULTI_VALUE_VARIABLE<STATE_VARIABLE>(pConfigNode, "Input-State", "State variables used as input of the Neural Network");
	m_inputAction = MULTI_VALUE_VARIABLE<ACTION_VARIABLE>(pConfigNode, "Input-Action", "Action variables used as input of the Neural Network");

	//add state/action variables to the list stored by the base-class DeepNetworkDefinition
	for (size_t i = 0; i < m_inputState.size(); i++)
		m_inputStateVariables.push_back(m_inputState[i]->get());
	for (size_t i = 0; i < m_inputAction.size(); i++)
		m_inputActionVariables.push_back(m_inputState[i]->get());

	//Only one output: Q(s,a)
	m_numOutputs = 1;
}

IContinuousQFunctionNetwork* DeepContinuousQFunction::getNetworkInstance()
{
	return CNTK::WrapperClient::getContinuousQFunctionNetwork(m_inputStateVariables.size(), m_inputActionVariables.size()
		, getLayersDefinition(), getLearnerDefinition(), m_useMinibatchNormalization.get());
}


DeepVFunction::DeepVFunction() {}
DeepVFunction::DeepVFunction(ConfigNode* pConfigNode) : DeepNetworkDefinition(pConfigNode)
{
	m_inputState = MULTI_VALUE_VARIABLE<STATE_VARIABLE>(pConfigNode, "Input-State", "State variables used as input of the Neural Network");
	
	//add state variables to the list stored by the base-class DeepNetworkDefinition
	for (size_t i = 0; i < m_inputState.size(); i++)
		m_inputStateVariables.push_back(m_inputState[i]->get());

	//Only one output: V(s)
	m_numOutputs = 1;
}

IVFunctionNetwork* DeepVFunction::getNetworkInstance()
{
	return CNTK::WrapperClient::getVFunctionNetwork(m_inputStateVariables.size()
		, getLayersDefinition(), getLearnerDefinition(), m_useMinibatchNormalization.get());
}


DeepDeterministicPolicy::DeepDeterministicPolicy() {}
DeepDeterministicPolicy::DeepDeterministicPolicy(ConfigNode* pConfigNode) : DeepNetworkDefinition(pConfigNode)
{
	//add state variables to the list stored by the base-class DeepNetworkDefinition
	m_inputState = MULTI_VALUE_VARIABLE<STATE_VARIABLE>(pConfigNode, "Input-State", "State variables used as input of the Neural Network");
	m_outputAction = MULTI_VALUE_VARIABLE<ACTION_VARIABLE>(pConfigNode, "Output-Action", "Action variables used as output of the policy");

	//add state variables to the list stored by the base-class DeepNetworkDefinition
	for (size_t i = 0; i < m_inputState.size(); i++)
		m_inputStateVariables.push_back(m_inputState[i]->get());

	//One output per output action: V(s)
	m_numOutputs = m_outputAction.size();
}

IDeterministicPolicyNetwork* DeepDeterministicPolicy::getNetworkInstance()
{
	return CNTK::WrapperClient::getDeterministicPolicyNetwork(m_inputStateVariables.size()
		, getLayersDefinition(), getLearnerDefinition(), m_useMinibatchNormalization.get());
}