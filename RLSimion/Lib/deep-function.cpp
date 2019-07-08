#include "deep-function.h"


DeepFunction::DeepFunction(ConfigNode* pConfigNode)
{
	m_layers= MULTI_VALUE<DeepLayer>(pConfigNode, "Layers", "Layers of the Neural Network");
	m_learner = ENUM_PARAM<DeepLearner>(pConfigNode, "Learner", "Learner used for this Neural Network", DeepLearner::Adam);
	m_useMinibatchNormalization = BOOL_PARAM(pConfigNode, "Use-Normalization", "Use minibatch normalization", false);
	m_minibatchSize = INT_PARAM(pConfigNode, "Minibatch-Size", "Number of tuples in each minibatch used in updates", 100);
	m_learningRate = CHILD_OBJECT_FACTORY<NumericValue>(pConfigNode, "Learning-Rate", "Learning rate used to train the Neural Network");
}

DeepDiscreteQFunction::DeepDiscreteQFunction() {}
DeepDiscreteQFunction::DeepDiscreteQFunction(ConfigNode* pConfigNode) : DeepFunction(pConfigNode)
{
	m_inputState= MULTI_VALUE_VARIABLE<STATE_VARIABLE>(pConfigNode, "Input-State", "State variables used as input of the Neural Network");
	m_outputAction= MULTI_VALUE_VARIABLE<ACTION_VARIABLE>(pConfigNode, "Output-Action", "Action variables used as output of the Neural Network");
	m_numActionSteps= INT_PARAM(pConfigNode, "Num-Action-Steps", "Number of steps used to discretize each of the output action variables", 100);
}

void DeepDiscreteQFunction::deferredLoadStep()
{
	//add state variables to the list stored by the base-class DeepFunction
	for (int i = 0; i < m_inputState.size(); i++) m_inputStateVariables.push_back(m_inputState[i]->get());
	//calculate the total number of discretized actions and set it
	m_totalNumActionSteps = 1;
	for (size_t i = 0; i < m_outputAction.size(); i++)
		m_totalNumActionSteps *= m_numActionSteps.get();
	
	//One output for each discretized action
	m_numOutputs = m_totalNumActionSteps;
}

DeepContinuousQFunction::DeepContinuousQFunction() {}
DeepContinuousQFunction::DeepContinuousQFunction(ConfigNode* pConfigNode) : DeepFunction(pConfigNode)
{
	//add state/action variables to the lists stored by the base-class DeepFunction
	m_inputState = MULTI_VALUE_VARIABLE<STATE_VARIABLE>(pConfigNode, "Input-State", "State variables used as input of the Neural Network");
	m_inputAction = MULTI_VALUE_VARIABLE<ACTION_VARIABLE>(pConfigNode, "Input-Action", "Action variables used as input of the Neural Network");
}

void DeepContinuousQFunction::deferredLoadStep()
{
	for (int i = 0; i < m_inputState.size(); i++) m_inputStateVariables.push_back(m_inputState[i]->get());
	for (int i = 0; i < m_inputAction.size(); i++) m_inputActionVariables.push_back(m_inputState[i]->get());
	
	//Only one output: Q(s,a)
	m_numOutputs = 1;
}

DeepVFunction::DeepVFunction() {}
DeepVFunction::DeepVFunction(ConfigNode* pConfigNode) : DeepFunction(pConfigNode)
{
	m_inputState = MULTI_VALUE_VARIABLE<STATE_VARIABLE>(pConfigNode, "Input-State", "State variables used as input of the Neural Network");
}

void DeepVFunction::deferredLoadStep()
{
	//add state variables to the list stored by the base-class DeepFunction
	for (int i = 0; i < m_inputState.size(); i++) m_inputStateVariables.push_back(m_inputState[i]->get());
	
	//Only one output: V(s)
	m_numOutputs = 1;
}

DeepDeterministicPolicy::DeepDeterministicPolicy() {}
DeepDeterministicPolicy::DeepDeterministicPolicy(ConfigNode* pConfigNode) : DeepFunction(pConfigNode)
{
	//add state variables to the list stored by the base-class DeepFunction
	m_inputState = MULTI_VALUE_VARIABLE<STATE_VARIABLE>(pConfigNode, "Input-State", "State variables used as input of the Neural Network");
	m_outputAction = MULTI_VALUE_VARIABLE<ACTION_VARIABLE>(pConfigNode, "Output-Action", "Action variables used as output of the policy");
}

void DeepDeterministicPolicy::deferredLoadStep()
{
	for (int i = 0; i < m_inputState.size(); i++) m_inputStateVariables.push_back(m_inputState[i]->get());

	//One output per output action: V(s)
	m_numOutputs = m_outputAction.size();
}