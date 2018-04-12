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

void outputArguments(wstring header,FunctionPtr f)
{
	return;
	wcout << header << "\n";
	for each (auto arg in f->Arguments())
		wcout << arg.Name() << "\n";
	wcout << "\n";
}

void outputParameters(wstring header, FunctionPtr f)
{
	return;
	wcout << header << "\n";
	for each (auto arg in f->Parameters())
		wcout << arg.Name() << ": " << arg.AsString() << "\n";
	wcout << "\n";
}

void Network::buildNetwork(double learningRate)
{
	findInputsAndOutputs();

	m_targetVariable = CNTK::InputVariable({(size_t)m_pNetworkDefinition->getOutputSize()}
		, CNTK::DataType::Double, m_targetName);
	m_lossFunctionPtr = CNTK::SquaredError(m_QFunctionPtr, m_targetVariable, m_lossName);
	m_networkFunctionPtr = CNTK::Combine({ m_lossFunctionPtr, m_QFunctionPtr }, m_networkName);

	//trainer
	const OptimizerSettings* optimizer = m_pNetworkDefinition->getOptimizerSettings();
	m_trainer = CNTKWrapper::CreateOptimizer(optimizer, m_QFunctionPtr, m_lossFunctionPtr, learningRate);

	outputArguments(L"QFunctionPtr",m_QFunctionPtr);
	FunctionPtr foundPtr = m_networkFunctionPtr->FindByName(m_pNetworkDefinition->getOutputLayer())->Output();
	outputArguments(L"Found QFunctionPtr", foundPtr);
}

INetwork* Network::getFrozenCopy() const
{
	Network* result = new Network(m_pNetworkDefinition);

	outputArguments(L"Original Network:",m_QFunctionPtr);

	result->m_networkFunctionPtr = m_networkFunctionPtr->Clone(ParameterCloningMethod::Freeze);
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

	result->m_QFunctionPtr = result->m_networkFunctionPtr->FindByName(
		m_pNetworkDefinition->getOutputLayer())->Output();

	outputArguments(L"Cloned Network", result->m_QFunctionPtr);
	outputParameters(L"Original QNetwork", m_QFunctionPtr);
	outputParameters(L"Cloned QNetwork- parameters:", result->m_QFunctionPtr);

	result->m_lossFunctionPtr = m_networkFunctionPtr->FindByName(m_lossName);

	return result;
}

void Network::findInputsAndOutputs()
{
	if (m_QFunctionPtr == nullptr)
		throw runtime_error("Output layer not set in the network");
	//look for the input layer among the arguments
	for each(auto inputVariable in m_QFunctionPtr->Arguments())
	{
		wstring name = inputVariable.Name();
		wstring asstring = inputVariable.AsString();
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
	unordered_map<CNTK::Variable, CNTK::ValuePtr> arguments =
		unordered_map<CNTK::Variable, CNTK::ValuePtr>();
	//set inputs
	if (m_bInputStateUsed)
	{
		arguments[m_inputState] = CNTK::Value::CreateBatch(m_inputState.Shape()
			, pMinibatch->getInputState(), CNTK::DeviceDescriptor::CPUDevice());
	}
	if (m_bInputActionUsed)
	{
		arguments[m_inputAction] = CNTK::Value::CreateBatch(m_inputAction.Shape()
			, pMinibatch->getInputAction(), CNTK::DeviceDescriptor::CPUDevice());
	}

	//set target outputs
	arguments[m_targetVariable] = CNTK::Value::CreateBatch(m_targetVariable.Shape()
		, pMinibatch->getOutput(), CNTK::DeviceDescriptor::CPUDevice());
	//train the network using the minibatch
	m_trainer->TrainMinibatch(arguments, DeviceDescriptor::CPUDevice());

	//clear the minibatch
	pMinibatch->clear();
}

void Network::setOutputLayer(CNTK::FunctionPtr outputLayer)
{
	wstring name = outputLayer->Name();
	size_t size = outputLayer->Output().Shape().TotalSize();
	m_QFunctionPtr = outputLayer;
}

void Network::get(const State* s, const Action* a, vector<double>& outputVector)
{
	ValuePtr outputValue;
	size_t s1 = outputVector.size();
	size_t s2 = m_QFunctionPtr->Output().Shape().TotalSize();
	if (outputVector.size() % m_QFunctionPtr->Output().Shape().TotalSize())
	{
		throw runtime_error("predictionData does not have the right size.");
	}

	unordered_map<CNTK::Variable, CNTK::ValuePtr> outputs =
		{ { m_QFunctionPtr->Output(), outputValue } };

	unordered_map<CNTK::Variable, CNTK::ValuePtr> inputs =
		unordered_map<CNTK::Variable, CNTK::ValuePtr>();

	vector<double> inputState;
	
	if (m_bInputStateUsed)
	{
		const vector<size_t>& stateVars = m_pNetworkDefinition->getInputStateVarIds();
		inputState = vector<double>(stateVars.size());
		for (size_t i= 0; i< stateVars.size(); i++)
			inputState[i] = s->get((int)stateVars[i]);
		inputs[m_inputState] = CNTK::Value::CreateBatch(m_inputState.Shape()
			, inputState, CNTK::DeviceDescriptor::CPUDevice());
	}
	vector<double> inputAction;
	
	if (m_bInputActionUsed)
	{
		const vector<size_t>& actionVars = m_pNetworkDefinition->getInputActionVarIds();
		inputAction = vector<double>(actionVars.size());
		for (size_t i = 0; i < actionVars.size(); i++)
			inputAction[i] = s->get((int)actionVars[i]);
		inputs[m_inputAction] = CNTK::Value::CreateBatch(m_inputAction.Shape()
			, inputAction, CNTK::DeviceDescriptor::CPUDevice());
	}

	m_QFunctionPtr->Evaluate(inputs, outputs, CNTK::DeviceDescriptor::CPUDevice());

	outputValue = outputs[m_QFunctionPtr];

	CNTK::NDShape outputShape = m_QFunctionPtr->Output().Shape().AppendShape({ 1
		, outputVector.size() / m_QFunctionPtr->Output().Shape().TotalSize() });

	CNTK::NDArrayViewPtr cpuArrayOutput = CNTK::MakeSharedObject<CNTK::NDArrayView>(outputShape
		, outputVector, false);
	cpuArrayOutput->CopyFrom(*outputValue->Data());
}
/*
void Network::train(unordered_map<string, vector<double>&>& inputDataMap, vector<double>& targetOutputData)
{
	ValuePtr outputSequence = CNTK::Value::CreateBatch(m_targetVariable.Shape()
		, targetOutputData, CNTK::DeviceDescriptor::CPUDevice());

	unordered_map<CNTK::Variable, CNTK::ValuePtr> arguments = 
		unordered_map<CNTK::Variable, CNTK::ValuePtr>();

	vector<double> inputVector = vector<double>(m_pNetworkDefinition->getNumInputStateVars());

	//THIS WON'T WORK IF THERE IS MORE THAN ONE INPUT VARIABLE
	for each(auto inputVariable in m_inputs)
	{
		arguments[inputVariable] = CNTK::Value::CreateBatch(inputVariable.Shape()
			, inputVector, CNTK::DeviceDescriptor::CPUDevice());
	}

	arguments[m_targetVariable] = outputSequence;

	m_trainer->TrainMinibatch(arguments, DeviceDescriptor::CPUDevice());
}

void Network::gradients(unordered_map<string, vector<double>&>& inputDataMap
	, vector<double>& targetOutputData, unordered_map<CNTK::Variable
	, CNTK::ValuePtr>& gradients)
{
	FunctionPtr outputPtr = m_QFunctionPtr;
	ValuePtr outputValue;
	unordered_map<CNTK::Variable, CNTK::ValuePtr> outputs =
		{ { outputPtr->Output(), outputValue } ,{ m_lossFunctionPtr , nullptr } };

	unordered_map<CNTK::Variable, CNTK::ValuePtr> inputs =
		unordered_map<CNTK::Variable, CNTK::ValuePtr>();

	vector<double> inputVector = vector<double>(m_pNetworkDefinition->getNumInputStateVars());
	for each(auto inputVariable in m_inputs)
	{
		inputs[inputVariable] = CNTK::Value::CreateBatch(inputVariable.Shape()
			, inputVector, CNTK::DeviceDescriptor::CPUDevice());
	}
	auto backpropState = outputPtr->Forward(inputs, outputs, CNTK::DeviceDescriptor::CPUDevice()
		, { m_lossFunctionPtr });
	auto rootGradientValue =
		MakeSharedObject<Value>(MakeSharedObject<NDArrayView>(m_lossFunctionPtr->Output().GetDataType(),
		m_lossFunctionPtr->Output().Shape(), CNTK::DeviceDescriptor::CPUDevice()), outputs.at(m_lossFunctionPtr)->Mask());

	rootGradientValue->Data()->SetValue(1.0);

	outputPtr->Backward(backpropState, { { m_lossFunctionPtr,rootGradientValue } }, gradients);
}

void Network::gradients(unordered_map<string, vector<double>&>& inputDataMap
	, unordered_map<CNTK::Variable, CNTK::ValuePtr>& gradients)
{
	FunctionPtr outputPtr = m_QFunctionPtr;
	ValuePtr outputValue;

	unordered_map<CNTK::Variable, CNTK::ValuePtr> outputs =
		{ { outputPtr->Output(), outputValue } };
	unordered_map<CNTK::Variable, CNTK::ValuePtr> inputs =
		unordered_map<CNTK::Variable, CNTK::ValuePtr>();

	vector<double> inputVector = vector<double>(m_pNetworkDefinition->getNumInputStateVars());

	for each(auto inputVariable in m_inputs)
	{
		inputs[inputVariable] = CNTK::Value::CreateBatch(inputVariable.Shape()
			, inputVector, CNTK::DeviceDescriptor::CPUDevice());
	}

	outputPtr->Gradients(inputs, gradients);
}

void Network::predict(unordered_map<string, vector<double>&>& inputDataMap
	, vector<double>& predictionData)
{
	FunctionPtr outputPtr = m_QFunctionPtr;
	ValuePtr outputValue;

	unordered_map<CNTK::Variable, CNTK::ValuePtr> outputs =
	{ { outputPtr->Output(), outputValue } };

	unordered_map<CNTK::Variable, CNTK::ValuePtr> inputs =
		unordered_map<CNTK::Variable, CNTK::ValuePtr>();

	vector<double> inputVector = vector<double>(m_pNetworkDefinition->getNumInputStateVars());

	for each(auto inputVariable in m_inputs)
	{
		inputs[inputVariable] = CNTK::Value::CreateBatch(inputVariable.Shape()
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
}*/

#endif // _WIN64