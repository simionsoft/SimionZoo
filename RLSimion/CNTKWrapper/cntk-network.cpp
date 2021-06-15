#include "cntk-network.h"
#include "../Lib/deep-minibatch.h"
#include "../Lib/deep-functions.h"
#include "../../tools/System/CrossPlatform.h"
#include <vector>

#include "CNTKLibrary.h"
using namespace CNTK;


// CntkNetwork

CntkNetwork::CntkNetwork(vector<string> inputStateVariables, vector<string> inputActionVariables
	,size_t numOutputs, string networkLayersDefinition, string learnerDefinition, bool useNormalization)
{
	m_useNormalization = useNormalization;
	m_inputStateVariables = inputStateVariables;
	m_inputActionVariables = inputActionVariables;
	m_numOutputs = numOutputs;
	m_networkLayersDefinition = networkLayersDefinition;
	m_learnerDefinition = learnerDefinition;

	m_outputBuffer = vector<double>(numOutputs);
	m_stateBuffer = vector<double>(m_inputStateVariables.size());
}

unsigned int CntkNetwork::getNumOutputs()
{
	return (unsigned int) m_numOutputs;
}

const vector<string>& CntkNetwork::getInputStateVariables()
{
	return m_inputStateVariables;
}

const vector<string>& CntkNetwork::getInputActionVariables()
{
	return m_inputActionVariables;
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

CNTK::FunctionPtr CntkNetwork::denseLayer(CNTK::FunctionPtr layerInput, size_t outputDim, wstring layerId)
{
	size_t inputDim = layerInput->Output().Shape()[0];

	auto timesParam = Parameter({ outputDim, inputDim }
		, DataType::Double, GlorotUniformInitializer());
	auto timesFunction = Times(timesParam, layerInput, L"times");

	auto plusParam = Parameter({ outputDim }, 0.0);
	return Plus(plusParam, timesFunction, layerId);
}

CNTK::FunctionPtr CntkNetwork::normalizationLayer(CNTK::FunctionPtr layerInput)
{
	Variable biasParams = Parameter({ NDShape::InferredDimension }, 0.0);
	Variable scaleParams = Parameter({ NDShape::InferredDimension }, 0.0);
	Variable runningMean = Parameter({ NDShape::InferredDimension }, 0.0);
	Variable runningInvStd = Parameter({ NDShape::InferredDimension }, 0.0);
	Variable runningCount = Parameter({ 1 }, 0.0);
	
	return BatchNormalization(layerInput, scaleParams, biasParams, runningMean, runningInvStd, runningCount, false);
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

			if (!m_useNormalization)
				output = activationFunction(denseLayer(output, numUnits), activation);
			else
			{
				//If used, the batch normalization layer goes between the activation function and the dense layer:
				//https://docs.microsoft.com/en-us/cognitive-toolkit/Hands-On-Labs-Image-Recognition#solution-3-adding-batchnormalization
				//https://docs.microsoft.com/en-us/cognitive-toolkit/brainscript-layers-reference#batchnormalizationlayer-layernormalizationlayer-stabilizerlayer
				output = activationFunction(normalizationLayer(denseLayer(output, numUnits)), activation);
			}
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
	vector<string> learnerParameters = CrossPlatform::split(learnerDefinition, DeepNetworkDefinition::learnerParameterDelimiter);
	//We assume (learnerParameters.size() >= 1)
	//At least, the algorithm used to _train the network

	//learning rates must be set before training the net, so no need to set the initial value here
	if (learnerParameters[0] == "SGD")
		m_learner = CNTK::SGDLearner(networkOutput->Parameters()
			, CNTK::TrainingParameterPerSampleSchedule(0.0001));
	else if (learnerParameters[0] == "MomentumSGD")
		m_learner = CNTK::MomentumSGDLearner(networkOutput->Parameters()
			, CNTK::TrainingParameterPerSampleSchedule(0.0001)
			, CNTK::TrainingParameterPerSampleSchedule(0.9));
	else //"Adam"
		m_learner = CNTK::AdamLearner(networkOutput->Parameters()
			, CNTK::TrainingParameterPerSampleSchedule(0.0001)
			, CNTK::TrainingParameterPerSampleSchedule(0.9));

	return CreateTrainer(networkOutput, lossFunction, evalFunction, { m_learner });
}

CNTK::FunctionPtr CntkNetwork::initNetworkLearner(string learnerDefinition)
{
	m_targetVariable = CNTK::InputVariable({ m_numOutputs }, CNTK::DataType::Double, m_targetVariableId);
	CNTK::FunctionPtr lossFunctionPtr = CNTK::SquaredError(m_networkOutput, m_targetVariable, m_lossVariableId);
	CNTK::FunctionPtr evalFunctionPtr = CNTK::SquaredError(m_networkOutput, m_targetVariable, m_evalVariableId);
	CNTK::FunctionPtr fullNetworkLearnerFunction = CNTK::Combine({ lossFunctionPtr, m_networkOutput }, m_fullNetworLearnerFunctionId);

	//Create the trainer for the network
	m_trainer = trainer(m_networkOutput, lossFunctionPtr, evalFunctionPtr, learnerDefinition);

	return fullNetworkLearnerFunction;
}

CNTK::FunctionPtr CntkNetwork::initNetworkFromInputLayer(CNTK::FunctionPtr inputLayer)
{
	CNTK::FunctionPtr lastLayer = initNetworkLayers(inputLayer, m_networkLayersDefinition);
	return denseLayer(lastLayer, getNumOutputs(), m_networkOutputId);
}

void CntkNetwork::_train(DeepMinibatch* pMinibatch, const vector<double>& target, double learningRate)
{
	//Reset the learning rate for this next batch
	m_learner->SetLearningRateSchedule(CNTK::TrainingParameterPerSampleSchedule(learningRate));

	unordered_map<CNTK::Variable, CNTK::MinibatchData> arguments = unordered_map<CNTK::Variable, CNTK::MinibatchData>();
	//set the state input
	arguments[m_inputState] = CNTK::Value::CreateBatch(m_inputState.Shape(), pMinibatch->s(), CNTK::DeviceDescriptor::UseDefaultDevice());

	//set target outputs
	arguments[m_targetVariable] = CNTK::Value::CreateBatch(m_targetVariable.Shape(), target, CNTK::DeviceDescriptor::UseDefaultDevice());
	//_train the network using the minibatch
	m_trainer->TrainMinibatch(arguments, DeviceDescriptor::UseDefaultDevice());
}

void CntkNetwork::_clone(const CntkNetwork* pSource, bool bFreezeWeights)
{
	ParameterCloningMethod cloneMethod;
	if (bFreezeWeights)
		cloneMethod = ParameterCloningMethod::Freeze;
	else
		cloneMethod = ParameterCloningMethod::Clone;

	m_fullNetworkLearnerFunction = pSource->m_fullNetworkLearnerFunction->Clone(cloneMethod);

	for (auto input : m_fullNetworkLearnerFunction->Arguments())
	{
		wstring name = input.Name();
		if (name == m_stateInputVariableId)
			m_inputState = input;
		else if (name == m_targetVariableId)
			m_targetVariable = input;
	}

	m_networkOutput = m_fullNetworkLearnerFunction->FindByName(m_networkOutputId)->Output();

	m_outputBuffer = vector<double>(m_numOutputs);
	m_stateBuffer = vector<double>(m_inputStateVariables.size());
	m_trainer = pSource->m_trainer;
	m_learner = pSource->m_learner;
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
	const vector<string>& stateVars = getInputStateVariables();
	size_t numStateVars = stateVars.size();

	for (size_t i = 0; i < numStateVars; i++)
		stateVector[i] = s->getNormalized(stateVars[i].c_str());
}

void CntkNetwork::_softUpdate(CntkNetwork* pSource, double alpha)
{
	int parameterLearnerIndex = 0;
	int parameterIndex = 0;

	vector<Parameter> sourceParams = pSource->m_networkOutput->Parameters();
	vector<Parameter> targetParams = m_networkOutput->Parameters();

	for (const Parameter& targetParameter: m_networkOutput->Parameters())
	{
	//	for (const Parameter& targetParameter : learner->Parameters())
		{
			//if (!targetParameter.IsInput() && !targetParameter.IsOutput())
			{
				Parameter sourceParameter = sourceParams[parameterIndex];

				NDShape shape = sourceParameter.Shape();
				vector<double> sourceParameterValue = vector<double>(shape.TotalSize());
				NDArrayViewPtr sourceParameterView =
					MakeSharedObject<NDArrayView>(sourceParameter.Shape(), sourceParameterValue, false);
				sourceParameterView->CopyFrom(*sourceParameter.GetValue());

				size_t targetParameterSize = targetParameter.Shape().TotalSize();
				vector<double> targetParameterValue = vector<double>(targetParameterSize);
				NDArrayViewPtr targetParameterView =
					MakeSharedObject<NDArrayView>(targetParameter.Shape(), targetParameterValue, false);
				targetParameterView->CopyFrom(*targetParameter.GetValue());

				for (int i = 0; i < sourceParameterValue.size(); i++)
					targetParameterValue[i] = targetParameterValue[i] * (1 - alpha) + sourceParameterValue[i] * (alpha);

				targetParameter.Value()->CopyFrom(*targetParameterView);
				//qParameterGradientCpuArrayView->CopyFrom(*gradientPtr->Data());
			}

			parameterIndex++;
		}
		parameterLearnerIndex++;
	}
}

// CntkDiscreteQFunctionNetwork

CntkDiscreteQFunctionNetwork::CntkDiscreteQFunctionNetwork(vector<string> inputStateVariables, size_t numActionSteps
	, string networkLayersDefinition, string learnerDefinition, bool useNormalization)
	: CntkNetwork(inputStateVariables, {}, numActionSteps, networkLayersDefinition, learnerDefinition, useNormalization)
{
	m_inputState = CNTK::InputVariable({ getInputStateVariables().size() }
		, CNTK::DataType::Double, false, m_stateInputVariableId);
	m_networkOutput = initNetworkFromInputLayer(m_inputState);
	m_fullNetworkLearnerFunction = initNetworkLearner(m_learnerDefinition);
}
void CntkDiscreteQFunctionNetwork::destroy() { delete this;}
IDeepNetwork* CntkDiscreteQFunctionNetwork::clone(bool bFreezeWeights) const
{
	CntkDiscreteQFunctionNetwork* pClone = new CntkDiscreteQFunctionNetwork(m_inputStateVariables
		, m_numOutputs, m_networkLayersDefinition, m_learnerDefinition, m_useNormalization);
	pClone->_clone(this, bFreezeWeights);
	return pClone;
}
void CntkDiscreteQFunctionNetwork::train(DeepMinibatch* pMinibatch, const vector<double>& target, double learningRate)
{
	CntkNetwork::_train(pMinibatch, target, learningRate);
}
void CntkDiscreteQFunctionNetwork::evaluate(const vector<double>& s, vector<double>& output)
{
	CntkNetwork::_evaluate(s, output);
}
vector<double>& CntkDiscreteQFunctionNetwork::evaluate(const State* s, const Action* a)
{
	return CntkNetwork::_evaluate(s);
}
void CntkDiscreteQFunctionNetwork::softUpdate(IDeepNetwork* pSource, double alpha)
{
	CntkNetwork::_softUpdate((CntkDiscreteQFunctionNetwork*) pSource, alpha);
}

// CntkContinuousQFunctionNetwork

CntkContinuousQFunctionNetwork::CntkContinuousQFunctionNetwork(vector<string> inputStateVariables, vector<string> inputActionVariables
	, string networkLayersDefinition, string learnerDefinition, bool useNormalization)
	: CntkNetwork(inputStateVariables, inputActionVariables, 1, networkLayersDefinition, learnerDefinition, useNormalization)
{
	//2 main differences in this constructor:
	// 1. We use the action as input and we need the gradient wrt to it
	// 2. We use a merge layer to merge state and action inputs
	m_actionBuffer = vector<double>(getInputActionVariables().size());

	m_inputState = CNTK::InputVariable({ getInputStateVariables().size() }
		, CNTK::DataType::Double, false, m_stateInputVariableId);
	m_inputAction = CNTK::InputVariable({ getInputActionVariables().size() }
		, CNTK::DataType::Double, true, m_actionInputVariableId ); //<- we will need the gradient wrt action for DDPG
	
	CNTK::FunctionPtr inputMergeLayer = mergeLayer(m_inputState, m_inputAction);
	m_networkOutput = initNetworkFromInputLayer(inputMergeLayer);

	m_fullNetworkLearnerFunction = initNetworkLearner(m_learnerDefinition);
}

void CntkContinuousQFunctionNetwork::destroy() { delete this; }
IDeepNetwork* CntkContinuousQFunctionNetwork::clone(bool bFreezeWeights) const
{
	CntkContinuousQFunctionNetwork* pClone = new CntkContinuousQFunctionNetwork(m_inputStateVariables, m_inputActionVariables
		, m_networkLayersDefinition, m_learnerDefinition, m_useNormalization);
	pClone->_clone(this, bFreezeWeights);

	//initialize CntkContinuousQFunctionNetwork-specific stuff
	pClone->m_actionBuffer = vector<double>(m_inputActionVariables.size());

	for (auto input : pClone->m_fullNetworkLearnerFunction->Arguments())
	{
		wstring name = input.Name();
		if (name == m_actionInputVariableId)
			pClone->m_inputAction = input;
	}
	return pClone;
}

void CntkContinuousQFunctionNetwork::train(DeepMinibatch* pMinibatch, const vector<double>& target, double learningRate)
{
	//Reset the learning rate for this next batch
	m_learner->ResetLearningRate(CNTK::TrainingParameterPerSampleSchedule(learningRate));

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
	inputs[m_inputAction] = CNTK::Value::CreateBatch(m_inputAction.Shape(), a, CNTK::DeviceDescriptor::UseDefaultDevice());

	m_networkOutput->Evaluate(inputs, outputs, CNTK::DeviceDescriptor::UseDefaultDevice());

	outputValue = outputs[m_networkOutput];

	CNTK::NDShape outputShape = m_networkOutput->Output().Shape().AppendShape({ 1
		, output.size() / m_networkOutput->Output().Shape().TotalSize() });

	CNTK::NDArrayViewPtr cpuArrayOutput = CNTK::MakeSharedObject<CNTK::NDArrayView>(outputShape, output, false);
	cpuArrayOutput->CopyFrom(*outputValue->Data());
}

void CntkContinuousQFunctionNetwork::actionToVector(const Action* a, vector<double>& actionVector)
{
	const vector<string>& actionVars = getInputActionVariables();
	size_t numActionVars = actionVars.size();

	for (size_t i = 0; i < numActionVars; i++)
		actionVector[i] = a->getNormalized(actionVars[i].c_str());
}

vector<double>& CntkContinuousQFunctionNetwork::evaluate(const State* s, const Action* a)
{
	stateToVector(s, m_stateBuffer);
	actionToVector(a, m_actionBuffer);
	evaluate(m_stateBuffer, m_actionBuffer, m_outputBuffer);
	return m_outputBuffer;
}

void CntkContinuousQFunctionNetwork::gradientWrtAction(const vector<double>& s, const vector<double>&a, vector<double>& gradient)
{
	unordered_map<Variable, ValuePtr> arguments = {};
	unordered_map<Variable, ValuePtr> gradients = {};

	arguments[m_inputState] = CNTK::Value::CreateBatch(m_inputState.Shape(), s, CNTK::DeviceDescriptor::UseDefaultDevice());
	arguments[m_inputAction] = CNTK::Value::CreateBatch(m_inputAction.Shape(), a, CNTK::DeviceDescriptor::UseDefaultDevice());

	gradients[m_inputAction] = nullptr;

	m_networkOutput->Gradients(arguments, gradients);

	//copy gradient to cpu vector
	ValuePtr gradientPtr = gradients[m_inputAction];

	NDArrayViewPtr qParameterGradientCpuArrayView = MakeSharedObject<NDArrayView>(gradientPtr->Shape()
		, gradient, false);
	qParameterGradientCpuArrayView->CopyFrom(*gradientPtr->Data());
}

void CntkContinuousQFunctionNetwork::softUpdate(IDeepNetwork* pSource, double alpha)
{
	CntkNetwork::_softUpdate((CntkContinuousQFunctionNetwork*)pSource, alpha);
}

// CntkVFunctionNetwork

CntkVFunctionNetwork::CntkVFunctionNetwork(vector<string> inputStateVariables, string networkLayersDefinition, string learnerDefinition, bool useNormalization)
	: CntkNetwork(inputStateVariables, {}, 1, networkLayersDefinition, learnerDefinition, useNormalization)
{
	m_inputState = CNTK::InputVariable({ getInputStateVariables().size() }
		, CNTK::DataType::Double, false, m_stateInputVariableId);
	m_networkOutput = initNetworkFromInputLayer(m_inputState);
	m_fullNetworkLearnerFunction = initNetworkLearner(m_learnerDefinition);
}

void CntkVFunctionNetwork::destroy() { delete this; }
IDeepNetwork* CntkVFunctionNetwork::clone(bool bFreezeWeights) const
{
	CntkVFunctionNetwork* pClone = new CntkVFunctionNetwork(m_inputStateVariables
		, m_networkLayersDefinition, m_learnerDefinition, m_useNormalization);
	pClone->_clone(this, bFreezeWeights);
	return pClone;
}
void CntkVFunctionNetwork::train(DeepMinibatch* pMinibatch, const vector<double>& target, double learningRate)
{
	CntkNetwork::_train(pMinibatch, target, learningRate);
}
void CntkVFunctionNetwork::evaluate(const vector<double>& s, vector<double>& output)
{
	CntkNetwork::_evaluate(s, output);
}
vector<double>& CntkVFunctionNetwork::evaluate(const State* s, const Action* a)
{
	return CntkNetwork::_evaluate(s);
}

void CntkVFunctionNetwork::softUpdate(IDeepNetwork* pSource, double alpha)
{
	CntkNetwork::_softUpdate((CntkVFunctionNetwork*)pSource, alpha);
}

// CntkDeterministicPolicyNetwork

CntkDeterministicPolicyNetwork::CntkDeterministicPolicyNetwork(vector<string> inputStateVariables, vector<string> outputActionVariables, string networkLayersDefinition, string learnerDefinition, bool useNormalization)
	: CntkNetwork(inputStateVariables, {}, outputActionVariables.size(), networkLayersDefinition, learnerDefinition, useNormalization)
{
	m_outputActionVariables = outputActionVariables;
	m_inputState = CNTK::InputVariable({ getInputStateVariables().size() }
		, CNTK::DataType::Double, false, m_stateInputVariableId);
	// Because we are using normalized actions, we send the network output through a sigmoid function so that the output is scaled to [0,1]
	m_networkOutput = CNTK::Sigmoid(initNetworkFromInputLayer(m_inputState), m_actionScaleFunctionId);
	
	m_fullNetworkLearnerFunction = initNetworkLearner(m_learnerDefinition);
}
void CntkDeterministicPolicyNetwork::destroy() { delete this; }
IDeepNetwork* CntkDeterministicPolicyNetwork::clone(bool bFreezeWeights) const
{
	CntkDeterministicPolicyNetwork* pClone = new CntkDeterministicPolicyNetwork(m_inputStateVariables
		, m_outputActionVariables, m_networkLayersDefinition, m_learnerDefinition, m_useNormalization);
	pClone->_clone(this, bFreezeWeights);

	//Fix the output: with deterministic policy networks, the layer named "output" is not the real output because we are using an additional sigmoid function after it
	pClone->m_networkOutput = pClone->m_fullNetworkLearnerFunction->FindByName(m_actionScaleFunctionId)->Output();
	return pClone;
}
unsigned int CntkDeterministicPolicyNetwork::getNumOutputs()
{
	return (unsigned int) m_outputActionVariables.size();
}
void CntkDeterministicPolicyNetwork::train(DeepMinibatch* pMinibatch, const vector<double>& target, double learningRate)
{
	CntkNetwork::_train(pMinibatch, target, learningRate);
}
void CntkDeterministicPolicyNetwork::evaluate(const vector<double>& s, vector<double>& output)
{
	CntkNetwork::_evaluate(s, output);
}
vector<double>& CntkDeterministicPolicyNetwork::evaluate(const State* s, const Action* a)
{
	return CntkNetwork::_evaluate(s);
}

void CntkDeterministicPolicyNetwork::softUpdate(IDeepNetwork* pSource, double alpha)
{
	CntkNetwork::_softUpdate((CntkDeterministicPolicyNetwork*)pSource, alpha);
}

void CntkDeterministicPolicyNetwork::applyGradient(const vector<double>& s, const vector<double>& target)
{
	//Similar to the actual training function in https://github.com/Microsoft/CNTK/blob/94e6582d2f63ce3bb048b9da01679abeacda877f/Source/CNTKv2LibraryDll/Trainer.cpp#L193
	//but with a different root value (taken from the minibatch) that, in the case of DDPG, should be -dQ(s,a)/da
	//Forward pass
	unordered_map<Variable, ValuePtr> arguments = {};
	unordered_map<Variable, ValuePtr> output = {};

	arguments[m_inputState] = CNTK::Value::CreateBatch(m_inputState.Shape(), s, CNTK::DeviceDescriptor::UseDefaultDevice());

	output[m_networkOutput] = nullptr;

	auto backPropState = m_networkOutput->Forward(arguments, output, DeviceDescriptor::UseDefaultDevice(), { m_networkOutput });

	//Backward pass
	vector<double> RootValue = vector<double>(target.size());
	auto RootGradientValue = MakeSharedObject<Value>(MakeSharedObject<NDArrayView>(
		m_networkOutput->Output().Shape().AppendShape({ 1, RootValue.size() / m_networkOutput->Output().Shape().TotalSize() })
		, RootValue, false));

	for (int i = 0; i < target.size(); i++)
		RootValue[i] = target[i];

	unordered_map<Variable, ValuePtr> parameterGradients = {};
	//parameterGradients[m_inputState] = nullptr;
	for (const LearnerPtr& learner : m_trainer->ParameterLearners())
		for (const Parameter& parameter : learner->Parameters())
			parameterGradients[parameter] = nullptr;

	m_networkOutput->Backward(backPropState, { { m_networkOutput, RootGradientValue } }, parameterGradients);

	std::unordered_map<Parameter, NDArrayViewPtr> gradientsData = {};
	for (const LearnerPtr& learner : m_trainer->ParameterLearners())
	{
		for (const auto& parameter : learner->Parameters())
			gradientsData[parameter] = parameterGradients[parameter]->Data();

		learner->Update(gradientsData, target.size(), false);
	}
}
