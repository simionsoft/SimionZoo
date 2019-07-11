#include "cntk-network.h"
#include "../Lib/deep-functions.h"
#include "../Lib/deep-minibatch.h"
#include "CNTKLibrary.h"
#include "CNTKWrapperInternals.h"
#include "../../tools/System/CrossPlatform.h"
#include <vector>

// CntkNetwork

CntkNetwork::CntkNetwork(DeepNetworkDefinition* pNetworkDefinition)
{
	m_pDefinition = pNetworkDefinition;
	m_outputBuffer = vector<double>(pNetworkDefinition->getNumOutputs());
	m_stateBuffer = vector<double>(pNetworkDefinition->getInputStateVariables().size());
}

unsigned int CntkNetwork::getNumOutputs()
{
	return m_pDefinition->getNumOutputs();
}

const vector<string>& CntkNetwork::getInputStateVariables()
{
	return m_pDefinition->getInputStateVariables();
}

const vector<string>& CntkNetwork::getInputActionVariables()
{
	return m_pDefinition->getInputActionVariables();
}

CNTK::FunctionPtr CntkNetwork::activationFunction(CNTK::FunctionPtr input, Activation activationFunction)
{
	switch (activationFunction)
	{
	case Activation::ELU:
		return CNTK::ELU(input, L"ELU");
	case Activation::ReLU:
		return CNTK::ReLU(input, L"ReLu");
	case Activation::Sigmoid:
		return CNTK::Sigmoid(input, L"Sigmoid");
	case Activation::SoftMax:
		return CNTK::Softmax(input, L"Softmax");
	case Activation::SoftPlus:
		return CNTK::Softplus(input, L"Softplus");
	case Activation::Tanh:
		return CNTK::Tanh(input, L"Tanh");
	case Activation::Linear:
	default:
		return input;
	}
}

CNTK::FunctionPtr CntkNetwork::denseLayer(CNTK::FunctionPtr layerInput, size_t outputDim)
{
	size_t inputDim = layerInput->Output().Shape()[0];

	auto timesParam = Parameter({ outputDim, inputDim }
		, DataType::Double, GlorotUniformInitializer(DefaultParamInitScale
			, SentinelValueForInferParamInitRank, SentinelValueForInferParamInitRank, 1));
	auto timesFunction = Times(timesParam, layerInput, L"times");

	auto plusParam = Parameter({ outputDim }, 0.0);
	return Plus(plusParam, timesFunction);
}

CNTK::FunctionPtr CntkNetwork::initNetworkLayers(CNTK::FunctionPtr networkInput, string layersDefinition)
{
	CNTK::FunctionPtr output = networkInput;
	vector<string> layerDefinitions = CrossPlatform::split(layersDefinition, DeepNetworkDefinition::layerDefinitionDelimiter);
	for (int i = 0; i < layerDefinitions.size(); i++)
	{
		vector<string> layerParameters = CrossPlatform::split(layerDefinitions[i], DeepNetworkDefinition::layerParameterDelimiter);
		if (layerParameters.size() == 2) //for now, only 2 parameters: activation and #units
		{
			Activation activation = DeepLayer::activationFromFunctionName(layerParameters[0].c_str());
			int numUnits = std::atoi(layerParameters[1].c_str());
			output = activationFunction(denseLayer(output, numUnits), activation);
		}
	}
	return output;
}

CNTK::FunctionPtr CntkNetwork::mergeLayer(CNTK::Variable var1, CNTK::Variable var2)
{
	return Splice({ var1, var2 },CNTK::Axis(0));
}

CNTK::TrainerPtr CntkNetwork::trainer(CNTK::FunctionPtr networkOutput, CNTK::FunctionPtr lossFunction, CNTK::FunctionPtr evalFunction, string learnerDefinition)
{
	LearnerPtr learner;
	vector<string> learnerParameters = CrossPlatform::split(learnerDefinition, DeepNetworkDefinition::learnerParameterDelimiter);
	//We assume (learnerParameters.size() >= 1)
	//At least, the algorithm used to _train the network

	//learning rates must be set before training the net, so no need to set the initial value here
	if (learnerParameters[0] == "SGD")
		learner = CNTK::SGDLearner(networkOutput->Parameters()
			, CNTK::TrainingParameterPerSampleSchedule(0.0001));
	else if (learnerParameters[0] == "MomentumSGD")
		learner = CNTK::MomentumSGDLearner(networkOutput->Parameters()
			, CNTK::TrainingParameterPerSampleSchedule(0.0001)
			, CNTK::TrainingParameterPerSampleSchedule(0.9));
	else //"Adam"
		learner = CNTK::AdamLearner(networkOutput->Parameters()
			, CNTK::TrainingParameterPerSampleSchedule(0.0001)
			, CNTK::TrainingParameterPerSampleSchedule(0.9));

	return CreateTrainer(networkOutput, lossFunction, evalFunction, { learner });
}

CNTK::FunctionPtr CntkNetwork::initNetworkLearner(CNTK::FunctionPtr networkOutput, string learnerDefinition)
{
	m_targetVariable = CNTK::InputVariable({ m_pDefinition->getNumOutputs() }
										, CNTK::DataType::Double, m_targetVariableId);
	CNTK::FunctionPtr lossFunctionPtr = CNTK::SquaredError(m_networkOutput, m_targetVariable, m_lossVariableId);
	CNTK::FunctionPtr evalFunctionPtr = CNTK::SquaredError(m_networkOutput, m_targetVariable, m_lossVariableId);
	CNTK::FunctionPtr fullNetworkLearnerFunction = CNTK::Combine({ lossFunctionPtr, m_networkOutput }, m_fullNetworLearnerFunctionId);

	//Create the trainer for the network
	m_trainer = trainer(m_networkOutput, lossFunctionPtr, evalFunctionPtr, learnerDefinition);

	return fullNetworkLearnerFunction;
}

CNTK::FunctionPtr CntkNetwork::initNetworkFromInputLayer(CNTK::FunctionPtr inputLayer)
{
	CNTK::FunctionPtr lastLayer = initNetworkLayers(m_inputState, m_pDefinition->getLayersDefinition());
	m_networkOutput = denseLayer(lastLayer, getNumOutputs());
	return initNetworkLearner(m_networkOutput, m_pDefinition->getLearnerDefinition());
}

void CntkNetwork::_train(DeepMinibatch* pMinibatch, const vector<double>& target)
{
	unordered_map<CNTK::Variable, CNTK::MinibatchData> arguments = unordered_map<CNTK::Variable, CNTK::MinibatchData>();
	//set the state input
	arguments[m_inputState] = CNTK::Value::CreateBatch(m_inputState.Shape(), pMinibatch->s(), CNTK::DeviceDescriptor::UseDefaultDevice());

	//set target outputs
	arguments[m_targetVariable] = CNTK::Value::CreateBatch(m_targetVariable.Shape(), target, CNTK::DeviceDescriptor::UseDefaultDevice());
	//_train the network using the minibatch
	m_trainer->TrainMinibatch(arguments, DeviceDescriptor::UseDefaultDevice());
}

void CntkNetwork::_evaluate(const vector<double>& s, vector<double>& output)
{
	ValuePtr outputValue;

	unordered_map<CNTK::Variable, CNTK::ValuePtr> outputs = { { m_networkOutput->Output(), outputValue } };

	unordered_map<CNTK::Variable, CNTK::ValuePtr> inputs = {};

	size_t size = m_inputState.Shape().TotalSize();
	inputs[m_inputState] = CNTK::Value::CreateBatch(m_inputState.Shape(), s, CNTK::DeviceDescriptor::UseDefaultDevice());

	m_networkOutput->Evaluate(inputs, outputs, CNTK::DeviceDescriptor::UseDefaultDevice());

	outputValue = outputs[m_networkOutput];

	CNTK::NDShape outputShape = m_networkOutput->Output().Shape().AppendShape({ 1
		, output.size() / m_networkOutput->Output().Shape().TotalSize() });

	CNTK::NDArrayViewPtr cpuArrayOutput = CNTK::MakeSharedObject<CNTK::NDArrayView>(outputShape, output, false);
	cpuArrayOutput->CopyFrom(*outputValue->Data());
}

vector<double>& CntkNetwork::_evaluate(const State* s)
{
	stateToVector(s, m_stateBuffer);
	_evaluate(m_stateBuffer, m_outputBuffer);
	return m_outputBuffer;
}

void CntkNetwork::stateToVector(const State* s, vector<double>& stateVector)
{
	const vector<string>& stateVars = m_pDefinition->getInputStateVariables();
	size_t numStateVars = stateVars.size();

	for (size_t i = 0; i < numStateVars; i++)
		stateVector[i] = s->getNormalized(stateVars[i].c_str());
}

// CntkDiscreteQFunctionNetwork

CntkDiscreteQFunctionNetwork::CntkDiscreteQFunctionNetwork(DeepNetworkDefinition* pNetworkDefinition)
	: CntkNetwork(pNetworkDefinition)
{
	m_inputState = CNTK::InputVariable({ pNetworkDefinition->getInputStateVariables().size() }
		, CNTK::DataType::Double, false, m_stateInputVariableId);
	m_fullNetworkLearnerFunction = initNetworkFromInputLayer(m_inputState);
}
void CntkDiscreteQFunctionNetwork::destroy() { delete this;}
IDeepNetwork* CntkDiscreteQFunctionNetwork::clone(bool bFreezeWeights) const { return nullptr; }
void CntkDiscreteQFunctionNetwork::train(DeepMinibatch* pMinibatch, const vector<double>& target)
{
	CntkNetwork::_train(pMinibatch, target);
}
void CntkDiscreteQFunctionNetwork::evaluate(const vector<double>& s, vector<double>& output)
{
	CntkNetwork::_evaluate(s, output);
}
vector<double>& CntkDiscreteQFunctionNetwork::evaluate(const State* s, const Action* a)
{
	return CntkNetwork::_evaluate(s);
}

// CntkContinuousQFunctionNetwork

CntkContinuousQFunctionNetwork::CntkContinuousQFunctionNetwork(DeepNetworkDefinition* pNetworkDefinition)
	: CntkNetwork(pNetworkDefinition)
{
	m_actionBuffer = vector<double>(pNetworkDefinition->getInputActionVariables().size());

	m_inputState = CNTK::InputVariable({ pNetworkDefinition->getInputStateVariables().size() }
		, CNTK::DataType::Double, false, m_stateInputVariableId);
	m_inputAction = CNTK::InputVariable({ pNetworkDefinition->getInputActionVariables().size() }
		, CNTK::DataType::Double, false, m_actionInputVariableId);
	CNTK::FunctionPtr inputMergeLayer = mergeLayer(m_inputState, m_inputAction);

	m_fullNetworkLearnerFunction = initNetworkFromInputLayer(m_inputState);
}

void CntkContinuousQFunctionNetwork::destroy() { delete this; }
IDeepNetwork* CntkContinuousQFunctionNetwork::clone(bool bFreezeWeights) const { return nullptr; }

void CntkContinuousQFunctionNetwork::train(DeepMinibatch* pMinibatch, const vector<double>& target)
{
	unordered_map<CNTK::Variable, CNTK::MinibatchData> arguments = unordered_map<CNTK::Variable, CNTK::MinibatchData>();
	//set the state/action input
	arguments[m_inputState] = CNTK::Value::CreateBatch(m_inputState.Shape(), pMinibatch->s(), CNTK::DeviceDescriptor::UseDefaultDevice());
	arguments[m_inputAction] = CNTK::Value::CreateBatch(m_inputAction.Shape(), pMinibatch->a(), CNTK::DeviceDescriptor::UseDefaultDevice());

	//set target outputs
	arguments[m_targetVariable] = CNTK::Value::CreateBatch(m_targetVariable.Shape(), target, CNTK::DeviceDescriptor::UseDefaultDevice());
	//_train the network using the minibatch
	m_trainer->TrainMinibatch(arguments, DeviceDescriptor::UseDefaultDevice());
}

void CntkContinuousQFunctionNetwork::evaluate(const vector<double>& s, const vector<double>& a, vector<double>& output)
{
	ValuePtr outputValue;

	unordered_map<CNTK::Variable, CNTK::ValuePtr> outputs = { { m_networkOutput->Output(), outputValue } };

	unordered_map<CNTK::Variable, CNTK::ValuePtr> inputs = {};

	size_t size = m_inputState.Shape().TotalSize();
	inputs[m_inputState] = CNTK::Value::CreateBatch(m_inputState.Shape(), s, CNTK::DeviceDescriptor::UseDefaultDevice());
	inputs[m_inputAction] = CNTK::Value::CreateBatch(m_inputState.Shape(), a, CNTK::DeviceDescriptor::UseDefaultDevice());

	m_networkOutput->Evaluate(inputs, outputs, CNTK::DeviceDescriptor::UseDefaultDevice());

	outputValue = outputs[m_networkOutput];

	CNTK::NDShape outputShape = m_networkOutput->Output().Shape().AppendShape({ 1
		, output.size() / m_networkOutput->Output().Shape().TotalSize() });

	CNTK::NDArrayViewPtr cpuArrayOutput = CNTK::MakeSharedObject<CNTK::NDArrayView>(outputShape, output, false);
	cpuArrayOutput->CopyFrom(*outputValue->Data());
}

void CntkContinuousQFunctionNetwork::actionToVector(const Action* a, vector<double>& actionVector)
{
	const vector<string>& stateVars = m_pDefinition->getInputActionVariables();
	size_t numActionVars = stateVars.size();

	for (size_t i = 0; i < numActionVars; i++)
		actionVector[i] = a->getNormalized(stateVars[i].c_str());
}

vector<double>& CntkContinuousQFunctionNetwork::evaluate(const State* s, const Action* a)
{
	stateToVector(s, m_stateBuffer);
	actionToVector(a, m_actionBuffer);
	evaluate(m_stateBuffer, m_actionBuffer, m_outputBuffer);
	return m_outputBuffer;
}

// CntkVFunctionNetwork

CntkVFunctionNetwork::CntkVFunctionNetwork(DeepNetworkDefinition* pNetworkDefinition)
	: CntkNetwork(pNetworkDefinition)
{
	m_inputState = CNTK::InputVariable({ pNetworkDefinition->getInputStateVariables().size() }
		, CNTK::DataType::Double, false, m_stateInputVariableId);
	m_fullNetworkLearnerFunction = initNetworkFromInputLayer(m_inputState);
}

void CntkVFunctionNetwork::destroy() { delete this; }
IDeepNetwork* CntkVFunctionNetwork::clone(bool bFreezeWeightse) const { return nullptr; }
void CntkVFunctionNetwork::train(DeepMinibatch* pMinibatch, const vector<double>& target)
{
	CntkNetwork::_train(pMinibatch, target);
}
void CntkVFunctionNetwork::evaluate(const vector<double>& s, vector<double>& output)
{
	CntkNetwork::_evaluate(s, output);
}
vector<double>& CntkVFunctionNetwork::evaluate(const State* s, const Action* a)
{
	return CntkNetwork::_evaluate(s);
}

// CntkDeterministicPolicyNetwork

CntkDeterministicPolicyNetwork::CntkDeterministicPolicyNetwork(DeepNetworkDefinition* pNetworkDefinition)
	: CntkNetwork(pNetworkDefinition)
{
	m_inputState = CNTK::InputVariable({ pNetworkDefinition->getInputStateVariables().size() }
		, CNTK::DataType::Double, false, m_stateInputVariableId);
	m_fullNetworkLearnerFunction = initNetworkFromInputLayer(m_inputState);
}
void CntkDeterministicPolicyNetwork::destroy() { delete this; }
IDeepNetwork* CntkDeterministicPolicyNetwork::clone(bool bFreezeWeights) const { return nullptr; }
void CntkDeterministicPolicyNetwork::train(DeepMinibatch* pMinibatch, const vector<double>& target)
{
	CntkNetwork::_train(pMinibatch, target);
}
void CntkDeterministicPolicyNetwork::evaluate(const vector<double>& s, vector<double>& output)
{
	CntkNetwork::_evaluate(s, output);
}
vector<double>& CntkDeterministicPolicyNetwork::evaluate(const State* s, const Action* a)
{
	return CntkNetwork::_evaluate(s);
}