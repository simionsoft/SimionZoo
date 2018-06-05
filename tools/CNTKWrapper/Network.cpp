#ifdef _WIN64


#include "CNTKLibrary.h"
#include "Network.h"
#include "CNTKWrapperInternals.h"
#include "OptimizerSetting.h"
#include "InputData.h"
#include "NetworkDefinition.h"
#include <iostream>

Network::Network(NetworkDefinition* pNetworkDefinition)
{
	m_pNetworkDefinition = pNetworkDefinition;
}


Network::~Network()
{
}

void Network::destroy()
{
	delete this;
}

void Network::buildNetwork(double learningRate)
{
	findInputsAndOutputs();

	m_targetVariable = CNTK::InputVariable({(size_t)m_pNetworkDefinition->getOutputSize()}
		, CNTK::DataType::Double, m_targetName);
	m_lossFunctionPtr = CNTK::SquaredError(m_FunctionPtr, m_targetVariable, m_lossName);
	m_networkFunctionPtr = CNTK::Combine({ m_lossFunctionPtr, m_FunctionPtr }, m_networkName);

	//trainer
	const OptimizerSettings* optimizer = m_pNetworkDefinition->getOptimizerSettings();
	m_trainer = CNTKWrapper::CreateOptimizer(optimizer, m_FunctionPtr, m_lossFunctionPtr, learningRate);

	FunctionPtr foundPtr = m_networkFunctionPtr->FindByName(m_pNetworkDefinition->getOutputLayer())->Output();

	m_output = vector<double>(m_pNetworkDefinition->getOutputSize());
}

INetwork* Network::clone(bool bFreezeWeights) const
{
	Network* result = new Network(m_pNetworkDefinition);

	ParameterCloningMethod cloneMethod;
	if (bFreezeWeights)
		cloneMethod = ParameterCloningMethod::Freeze;
	else
		cloneMethod = ParameterCloningMethod::Clone;

	result->m_networkFunctionPtr = m_networkFunctionPtr->Clone(cloneMethod);

	for each (auto input in result->m_networkFunctionPtr->Arguments())
	{
		wstring name = input.Name();
		if (m_pNetworkDefinition->getStateInputLayer() == name)
		{
			result->m_inputState = input;
			result->m_bInputStateUsed = true;
		}
		else if (m_pNetworkDefinition->getActionInputLayer() == name)
		{
			result->m_inputAction = input;
			result->m_bInputActionUsed = true;
		}
		else //target layer
			result->m_targetVariable = input;
	}

	result->m_FunctionPtr = result->m_networkFunctionPtr->FindByName(
		m_pNetworkDefinition->getOutputLayer())->Output();

	result->m_lossFunctionPtr = m_networkFunctionPtr->FindByName(m_lossName);

	result->m_output = vector<double>(m_pNetworkDefinition->getOutputSize());

	return result;
}

void Network::initSoftUpdate(double u, INetwork* pTargetNetworkInterface)
{
	Network* pTargetNetwork = dynamic_cast<Network*>(pTargetNetworkInterface);
	if (!pTargetNetwork)
		throw std::exception("Incorrect target in CNTKWrapper::Network::initSoftUpdate");
	
	size_t numOnlineParams = m_FunctionPtr->Parameters().size();
	size_t numTargetParams = pTargetNetwork->m_FunctionPtr->Parameters().size();
	if (numOnlineParams!=numTargetParams)
		throw std::exception("Missmatched number of parameters in CNTKWrapper::Network::initSoftUpdate");

	auto scale = CNTK::Constant::Scalar(CNTK::DataType::Float, u, CNTK::DeviceDescriptor::UseDefaultDevice());
	auto anitScale = CNTK::Constant::Scalar(CNTK::DataType::Float, 1.0f - u, CNTK::DeviceDescriptor::UseDefaultDevice());

	for (int i = 0; i < m_FunctionPtr->Parameters().size(); i++)
	{
		auto targetParam = pTargetNetwork->m_FunctionPtr->Parameters()[i];
		auto predictionParam = m_FunctionPtr->Parameters()[i];

		auto weightTransition = CNTK::Plus(CNTK::ElementTimes(scale, predictionParam), CNTK::ElementTimes(anitScale, targetParam));
		m_weightTransitions[targetParam] = weightTransition;
	}
}

void Network::softUpdate(INetwork* pTargetNetworkInterface)
{
	Network* pTargetNetwork = dynamic_cast<Network*>(pTargetNetworkInterface);
	if (!pTargetNetwork)
		throw std::exception("Incorrect target in CNTKWrapper::Network::softUpdate");

	size_t numOnlineParams = m_FunctionPtr->Parameters().size();
	size_t numTargetParams = pTargetNetwork->m_FunctionPtr->Parameters().size();
	if (numOnlineParams != numTargetParams)
		throw std::exception("Missmatched number of parameters in CNTKWrapper::Network::softUpdate");

	for (int i = 0; i < m_FunctionPtr->Parameters().size(); i++)
	{
		auto targetParam = pTargetNetwork->m_FunctionPtr->Parameters()[i];
		auto weightTransition = m_weightTransitions[targetParam];

		CNTK::ValuePtr weightTransitionOutputValue;
		unordered_map<CNTK::Variable, CNTK::ValuePtr> outputs = { { weightTransition->Output(), weightTransitionOutputValue } };
		weightTransition->Evaluate({}, outputs, CNTK::DeviceDescriptor::UseDefaultDevice());
		weightTransitionOutputValue = outputs[weightTransition->Output()];

		targetParam.SetValue(weightTransitionOutputValue->Data());
	}
}

void Network::findInputsAndOutputs()
{
	if (m_FunctionPtr == nullptr)
		throw runtime_error("Output layer not set in the network");
	//look for the input layer among the arguments
	for each(auto inputVariable in m_FunctionPtr->Arguments())
	{
		//This avoids the "Target" layer to be added to the list
		if (m_pNetworkDefinition->getStateInputLayer() == inputVariable.Name())
		{
			m_inputState = inputVariable;
			m_bInputStateUsed = true;
		}
		else if (m_pNetworkDefinition->getActionInputLayer() == inputVariable.Name())
		{
			m_inputAction = inputVariable;
			m_bInputActionUsed = true;
		}
	}
}


void Network::save(string fileName)
{
	if (m_networkFunctionPtr == nullptr)
		throw std::runtime_error("Network has not been built yet. Call buildNetwork() before calling save().");
	m_networkFunctionPtr->Save(CNTKWrapper::Internal::string2wstring(fileName));
}


void Network::train(IMinibatch* pMinibatch)
{
	unordered_map<CNTK::Variable, CNTK::MinibatchData> arguments =
		unordered_map<CNTK::Variable, CNTK::MinibatchData>();
	//set inputs
	if (m_bInputStateUsed)
	{
		arguments[m_inputState] = CNTK::Value::CreateBatch(m_inputState.Shape()
			, pMinibatch->getInputState(), CNTK::DeviceDescriptor::UseDefaultDevice());
	}
	if (m_bInputActionUsed)
	{
		arguments[m_inputAction] = CNTK::Value::CreateBatch(m_inputAction.Shape()
			, pMinibatch->getInputAction(), CNTK::DeviceDescriptor::UseDefaultDevice());
	}

	//set target outputs
	arguments[m_targetVariable] = CNTK::Value::CreateBatch(m_targetVariable.Shape()
		, pMinibatch->getOutput(), CNTK::DeviceDescriptor::UseDefaultDevice());
	//train the network using the minibatch
	m_trainer->TrainMinibatch(arguments, DeviceDescriptor::UseDefaultDevice());

	//clear the minibatch
	pMinibatch->clear();
}

void Network::setOutputLayer(CNTK::FunctionPtr outputLayer)
{
	wstring name = outputLayer->Name();
	size_t size = outputLayer->Output().Shape().TotalSize();
	m_FunctionPtr = outputLayer;
}

void Network::stateToVector(const State* s, vector<double>& stateVector)
{
	const vector<string>& stateVars = m_pNetworkDefinition->getInputStateVariables();
	stateVector = vector<double>(stateVars.size());
	for (size_t i = 0; i< stateVars.size(); i++)
		stateVector[i] = s->getNormalized(stateVars[i].c_str());
}

void Network::actionToVector(const Action* a, vector<double>& actionVector)
{
	const vector<string>& actionVars = m_pNetworkDefinition->getInputActionVariables();
	actionVector = vector<double>(actionVars.size());
	for (size_t i = 0; i < actionVars.size(); i++)
		actionVector[i] = a->getNormalized(actionVars[i].c_str());
}

vector<double>& Network::evaluate(const State* s, const Action* a)
{
	ValuePtr outputValue;

	if (m_output.size() % m_FunctionPtr->Output().Shape().TotalSize())
		throw runtime_error("output vector does not have the right size.");

	unordered_map<CNTK::Variable, CNTK::ValuePtr> outputs =
		{ { m_FunctionPtr->Output(), outputValue } };

	unordered_map<CNTK::Variable, CNTK::ValuePtr> inputs = {};

	vector<double> inputState;
	if (m_bInputStateUsed)
	{
		stateToVector(s, inputState);
		inputs[m_inputState] = CNTK::Value::CreateBatch(m_inputState.Shape()
			, inputState, CNTK::DeviceDescriptor::UseDefaultDevice());
	}
	vector<double> inputAction;
	if (m_bInputActionUsed)
	{
		actionToVector(a, inputAction);
		inputs[m_inputAction] = CNTK::Value::CreateBatch(m_inputAction.Shape()
			, inputAction, CNTK::DeviceDescriptor::UseDefaultDevice());
	}

	m_FunctionPtr->Evaluate(inputs, outputs, CNTK::DeviceDescriptor::UseDefaultDevice());

	outputValue = outputs[m_FunctionPtr];

	CNTK::NDShape outputShape = m_FunctionPtr->Output().Shape().AppendShape({ 1
		, m_output.size() / m_FunctionPtr->Output().Shape().TotalSize() });

	CNTK::NDArrayViewPtr cpuArrayOutput = CNTK::MakeSharedObject<CNTK::NDArrayView>(outputShape
		, m_output, false);
	cpuArrayOutput->CopyFrom(*outputValue->Data());

	return m_output;
}

void Network::gradientWrtAction(const State* s, const Action* a, vector<double>& outputGradient)
{
	unordered_map<Variable, ValuePtr> arguments = {};
	unordered_map<Variable, ValuePtr> gradients = {};

	if (!m_bInputStateUsed || !m_bInputActionUsed)
		throw std::exception("Can only use gradient() with f(s,a)-form functions");

	vector<double> inputState;
	stateToVector(s, inputState);
	arguments[m_inputState] = CNTK::Value::CreateBatch(m_inputState.Shape()
		, inputState, CNTK::DeviceDescriptor::UseDefaultDevice());
	vector<double> inputAction;
	actionToVector(a, inputAction);
	arguments[m_inputAction] = CNTK::Value::CreateBatch(m_inputAction.Shape()
		, inputAction, CNTK::DeviceDescriptor::UseDefaultDevice());

	gradients[m_inputAction] = nullptr;

	m_FunctionPtr->Gradients(arguments, gradients);

	//copy gradient to cpu vector
	ValuePtr gradient = gradients[m_inputAction];
	if (gradient->Shape().TotalSize() != outputGradient.size())
		throw exception("Missmatched length for output vector in gradients()");

	NDArrayViewPtr qParameterGradientCpuArrayView =
		MakeSharedObject<NDArrayView>(gradient->Shape(), outputGradient, false);
	qParameterGradientCpuArrayView->CopyFrom(*gradient->Data());
}

void Network::applyGradient(IMinibatch* pMinibatch)
{
	//Similar to the actual training function in https://github.com/Microsoft/CNTK/blob/94e6582d2f63ce3bb048b9da01679abeacda877f/Source/CNTKv2LibraryDll/Trainer.cpp#L193
	//but with a different root value (taken from the minibatch) that, in the case of DDPG, should be -dQ(s,a)/da
	//Forward pass
	unordered_map<Variable, ValuePtr> arguments = {};
	unordered_map<Variable, ValuePtr> output = {};
	if (m_bInputStateUsed)
	{
		arguments[m_inputState] = CNTK::Value::CreateBatch(m_inputState.Shape()
			, pMinibatch->getInputState(), CNTK::DeviceDescriptor::UseDefaultDevice());
	}
	if (m_bInputActionUsed)
	{
		arguments[m_inputAction] = CNTK::Value::CreateBatch(m_inputAction.Shape()
			, pMinibatch->getInputAction(), CNTK::DeviceDescriptor::UseDefaultDevice());
	}
	output[m_FunctionPtr] = nullptr;
	auto backPropState = m_FunctionPtr->Forward(arguments, output, DeviceDescriptor::UseDefaultDevice(), { m_FunctionPtr });

	//Backward pass
	vector<double> RootValue = vector<double>(pMinibatch->outputSize()*pMinibatch->size());
	auto RootGradientValue = MakeSharedObject<Value>(MakeSharedObject<NDArrayView>(
		m_FunctionPtr->Output().Shape().AppendShape({ 1, RootValue.size() / m_FunctionPtr->Output().Shape().TotalSize() })
		, RootValue, false));

	for (int i = 0; i < pMinibatch->outputSize()*pMinibatch->size(); i++)
		RootValue[i] = pMinibatch->getOutput()[i];

	unordered_map<Variable, ValuePtr> parameterGradients = {};
	for (const LearnerPtr& learner : m_trainer->ParameterLearners())
		for (const Parameter& parameter : learner->Parameters())
			parameterGradients[parameter] = nullptr;
		
	m_FunctionPtr->Backward(backPropState, { { m_FunctionPtr, RootGradientValue } }, parameterGradients);

	std::unordered_map<Parameter, NDArrayViewPtr> gradientsData = {};
	for (const LearnerPtr& learner : m_trainer->ParameterLearners())
	{
		for (const auto& parameter : learner->Parameters())
			gradientsData[parameter] = parameterGradients[parameter]->Data();

		learner->Update(gradientsData, pMinibatch->size(), false);
	}
}

unsigned int Network::getNumOutputs()
{
	return (unsigned int) m_pNetworkDefinition->getOutputSize();
}

const vector<string>& Network::getInputStateVariables()
{
	return m_pNetworkDefinition->getInputStateVariables();
}

const vector<string>& Network::getInputActionVariables()
{
	return m_pNetworkDefinition->getInputActionVariables();
}
#endif // _WIN64