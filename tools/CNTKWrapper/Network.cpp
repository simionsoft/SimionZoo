#ifdef _WIN64


#include "CNTKLibrary.h"
#include "Network.h"
#include "CNTKWrapperInternals.h"
#include "OptimizerSetting.h"
#include "InputData.h"
#include "NetworkDefinition.h"

Network::Network(INetworkDefinition* pNetworkDefinition)
{
	m_pNetworkDefinition = pNetworkDefinition;
}


Network::~Network()
{
}

size_t Network::getTotalSize()
{
	return m_targetOutput.Shape().TotalSize();
}

void Network::destroy()
{
	delete this;
}

void Network::buildQNetwork()
{
	const OptimizerSettings* optimizer = m_pNetworkDefinition->getOptimizerSettings();

	//m_inputVariable = CNTK::InputVariable(getInputShape(), DataType::Double, L"Input");

	m_outputsFunctionPtr = vector<CNTK::FunctionPtr>();

	m_targetOutput = CNTK::InputVariable(m_targetOutput.Shape(), CNTK::DataType::Double, L"Target");

	//TODO: add support for different loss functions
	m_lossFunctionPtr = CNTK::SquaredError(m_outputsFunctionPtr.at(0), m_targetOutput, L"Loss");

	m_networkFunctionPtr = CNTK::Combine({ m_lossFunctionPtr, m_outputsFunctionPtr.at(0) }, L"Network");

	m_trainer = CNTKWrapper::CreateOptimizer(optimizer, m_outputsFunctionPtr.at(0), m_lossFunctionPtr);
}

void Network::save(string fileName)
{
	if (m_networkFunctionPtr == nullptr)
		throw std::runtime_error("Network has not been built yet. Call buildQNetwork() before calling save().");
	m_networkFunctionPtr->Save(CNTKWrapper::Internal::string2wstring(fileName));
}


void Network::train(IMinibatch* pMinibatch)
{
	unordered_map<CNTK::Variable, CNTK::ValuePtr> arguments =
		unordered_map<CNTK::Variable, CNTK::ValuePtr>();
	//set inputs
	for (auto inputVariable = m_pNetworkDefinition->getInputs().begin()
		; inputVariable != m_pNetworkDefinition->getInputs().end(); inputVariable++)
	{
		arguments[*inputVariable] = CNTK::Value::CreateBatch(inputVariable->Shape()
			, pMinibatch->getInputVector(), CNTK::DeviceDescriptor::CPUDevice());
	}
	//set target outputs
	arguments[m_targetOutput] = CNTK::Value::CreateBatch(m_targetOutput.Shape()
		, pMinibatch->getTargetOutputVector(), CNTK::DeviceDescriptor::CPUDevice());
	//train the network using the minibatch
	m_trainer->TrainMinibatch(arguments, DeviceDescriptor::CPUDevice());
}

void Network::get(State* s, vector<double>& outputVector)
{
	FunctionPtr outputPtr = getOutputsFunctionPtr().at(0);
	ValuePtr outputValue;

	if (outputVector.size() % outputPtr->Output().Shape().TotalSize())
	{
		throw runtime_error("predictionData does not have the right size.");
	}

	unordered_map<CNTK::Variable, CNTK::ValuePtr> outputs =
		{ { outputPtr->Output(), outputValue } };

	unordered_map<CNTK::Variable, CNTK::ValuePtr> inputs =
		unordered_map<CNTK::Variable, CNTK::ValuePtr>();

	vector<double> inputVector = vector<double>(m_pNetworkDefinition->getNumInputStateVars());
	for (size_t i = 0; i < m_pNetworkDefinition->getNumInputStateVars(); i++)
		inputVector[i] = s->get((int)m_pNetworkDefinition->getInputStateVar((int) i));

	//THIS WON'T WORK IF THERE IS MORE THAN 1 INPUT VARIABLE
	for (auto inputVariable = m_pNetworkDefinition->getInputs().begin()
		; inputVariable != m_pNetworkDefinition->getInputs().end(); inputVariable++)
	{
		inputs[*inputVariable] = CNTK::Value::CreateBatch(inputVariable->Shape()
			, inputVector, CNTK::DeviceDescriptor::CPUDevice());
	}
	outputPtr->Evaluate(inputs, outputs, CNTK::DeviceDescriptor::CPUDevice());

	outputValue = outputs[outputPtr];

	CNTK::NDShape outputShape = outputPtr->Output().Shape().AppendShape({ 1
		, outputVector.size() / outputPtr->Output().Shape().TotalSize() });

	CNTK::NDArrayViewPtr cpuArrayOutput = CNTK::MakeSharedObject<CNTK::NDArrayView>(outputShape
		, outputVector, false);
	cpuArrayOutput->CopyFrom(*outputValue->Data());
}

void Network::train(unordered_map<string, vector<double>&>& inputDataMap, vector<double>& targetOutputData)
{
	ValuePtr outputSequence = CNTK::Value::CreateBatch(m_targetOutput.Shape()
		, targetOutputData, CNTK::DeviceDescriptor::CPUDevice());

	unordered_map<CNTK::Variable, CNTK::ValuePtr> arguments = 
		unordered_map<CNTK::Variable, CNTK::ValuePtr>();

	vector<double> inputVector = vector<double>(m_pNetworkDefinition->getNumInputStateVars());

	//THIS WON'T WORK IF THERE IS MORE THAN ONE INPUT VARIABLE
	for (auto inputVariable = m_pNetworkDefinition->getInputs().begin()
		; inputVariable != m_pNetworkDefinition->getInputs().end(); inputVariable++)
	{
		arguments[*inputVariable] = CNTK::Value::CreateBatch(inputVariable->Shape()
			, inputVector, CNTK::DeviceDescriptor::CPUDevice());
	}

	arguments[m_targetOutput] = outputSequence;

	m_trainer->TrainMinibatch(arguments, DeviceDescriptor::CPUDevice());
}

void Network::gradients(unordered_map<string, vector<double>&>& inputDataMap
	, vector<double>& targetOutputData, unordered_map<CNTK::Variable
	, CNTK::ValuePtr>& gradients)
{
	FunctionPtr outputPtr = getOutputsFunctionPtr().at(0);
	ValuePtr outputValue;
	unordered_map<CNTK::Variable, CNTK::ValuePtr> outputs =
		{ { outputPtr->Output(), outputValue } ,{ m_lossFunctionPtr , nullptr } };

	unordered_map<CNTK::Variable, CNTK::ValuePtr> inputs =
		unordered_map<CNTK::Variable, CNTK::ValuePtr>();

	vector<double> inputVector = vector<double>(m_pNetworkDefinition->getNumInputStateVars());
	for (auto inputVariable = m_pNetworkDefinition->getInputs().begin()
		; inputVariable != m_pNetworkDefinition->getInputs().end(); inputVariable++)
	{
		inputs[*inputVariable] = CNTK::Value::CreateBatch(inputVariable->Shape()
			, inputVector, CNTK::DeviceDescriptor::CPUDevice());
	}
	auto backpropState = outputPtr->Forward(inputs, outputs, CNTK::DeviceDescriptor::CPUDevice()
		, { m_lossFunctionPtr });
	auto rootGradientValue =
		MakeSharedObject<Value>(MakeSharedObject<NDArrayView>(m_lossFunctionPtr->Output().GetDataType(),
		m_lossFunctionPtr->Output().Shape(), CNTK::DeviceDescriptor::CPUDevice()), outputs.at(m_lossFunctionPtr)->Mask());

	//to support double values later, too.
	if (m_lossFunctionPtr->Output().GetDataType() == DataType::Float)
		rootGradientValue->Data()->SetValue(1.0f);
	else
		rootGradientValue->Data()->SetValue(1.0);

	outputPtr->Backward(backpropState, { { m_lossFunctionPtr,rootGradientValue } }, gradients);
}

void Network::gradients(unordered_map<string, vector<double>&>& inputDataMap
	, unordered_map<CNTK::Variable, CNTK::ValuePtr>& gradients)
{
	FunctionPtr outputPtr = getOutputsFunctionPtr().at(0);
	ValuePtr outputValue;

	unordered_map<CNTK::Variable, CNTK::ValuePtr> outputs =
		{ { outputPtr->Output(), outputValue } };
	unordered_map<CNTK::Variable, CNTK::ValuePtr> inputs =
		unordered_map<CNTK::Variable, CNTK::ValuePtr>();

	vector<double> inputVector = vector<double>(m_pNetworkDefinition->getNumInputStateVars());

	for (auto inputVariable = m_pNetworkDefinition->getInputs().begin()
		; inputVariable != m_pNetworkDefinition->getInputs().end(); inputVariable++)
	{
		inputs[*inputVariable] = CNTK::Value::CreateBatch(inputVariable->Shape()
			, inputVector, CNTK::DeviceDescriptor::CPUDevice());
	}

	outputPtr->Gradients(inputs, gradients);
}

void Network::predict(unordered_map<string, vector<double>&>& inputDataMap
	, vector<double>& predictionData)
{
	FunctionPtr outputPtr = getOutputsFunctionPtr().at(0);
	ValuePtr outputValue;

	unordered_map<CNTK::Variable, CNTK::ValuePtr> outputs =
	{ { outputPtr->Output(), outputValue } };

	unordered_map<CNTK::Variable, CNTK::ValuePtr> inputs =
		unordered_map<CNTK::Variable, CNTK::ValuePtr>();

	vector<double> inputVector = vector<double>(m_pNetworkDefinition->getNumInputStateVars());

	for (auto inputVariable = m_pNetworkDefinition->getInputs().begin()
		; inputVariable != m_pNetworkDefinition->getInputs().end(); inputVariable++)
	{
		inputs[*inputVariable] = CNTK::Value::CreateBatch(inputVariable->Shape()
			, inputVector, CNTK::DeviceDescriptor::CPUDevice());
	}

	outputPtr->Evaluate(inputs, outputs, CNTK::DeviceDescriptor::CPUDevice());

	outputValue = outputs[outputPtr];

	if (predictionData.size() % outputPtr->Output().Shape().TotalSize())
	{
		throw runtime_error("predictionData does not have the right size.");
	}

	CNTK::NDShape outputShape = outputPtr->Output().Shape().AppendShape({ 1, predictionData.size() / outputPtr->Output().Shape().TotalSize() });

	CNTK::NDArrayViewPtr cpuArrayOutput = CNTK::MakeSharedObject<CNTK::NDArrayView>(outputShape, predictionData, false);
	cpuArrayOutput->CopyFrom(*outputValue->Data());
}

#include <iostream>
INetwork* Network::clone() const
{
	Network* result = new Network(m_pNetworkDefinition);
	result->m_trainer = nullptr;
	result->m_lossFunctionPtr = nullptr;

	result->m_networkFunctionPtr = m_networkFunctionPtr->Clone(CNTK::ParameterCloningMethod::Clone);

	for each (CNTK::Variable var in result->m_networkFunctionPtr->Outputs())
	{
		if (var.Name() != L"Loss" && var.Name() != L"loss")
			result->m_outputsFunctionPtr.push_back(var);
	}

	return result;
}


#endif // _WIN64