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

	////sanity check
	//wstring layerName = L"ActivationFunction";
	//FunctionPtr a = InputVariable({ 2 }, DataType::Double, "a");
	//FunctionPtr b = InputVariable({ 2 }, DataType::Double, "b");
	//FunctionPtr sum = Plus(a, b, L"plus");
	//FunctionPtr c = InputVariable({ 2 }, DataType::Double, "c");
	//FunctionPtr activationFunction = CNTK::ReLU(c, layerName);
	//wstring name1 = activationFunction->Name();


	//FunctionPtr times = Combine({ sum, activationFunction }, L"combine");

	//wstring asString1 = activationFunction->AsString();
	//wstring uid1 = activationFunction->Uid();

	//FunctionPtr clone = times->Clone();
	//FunctionPtr clonedActivationFunction = clone->FindByName(layerName);
	//wstring asString2 = clonedActivationFunction->AsString();
	//wstring name2 = clonedActivationFunction->Name();
	//wstring uid2 = clonedActivationFunction->Uid();
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
	wcout << header << "\n";
	for each (auto arg in f->Parameters())
		wcout << arg.Name() << ": " << arg.AsString() << "\n";
	wcout << "\n";
}

void Network::buildQNetwork()
{
	findInputsAndOutputs();

	m_targetVariable = CNTK::InputVariable({(size_t)m_pNetworkDefinition->getNumOutputs()}
		, CNTK::DataType::Double, m_targetName);
	m_lossFunctionPtr = CNTK::SquaredError(m_QFunctionPtr, m_targetVariable, m_lossName);
	m_networkFunctionPtr = CNTK::Combine({ m_lossFunctionPtr, m_QFunctionPtr }, m_networkName);

	//trainer
	const OptimizerSettings* optimizer = m_pNetworkDefinition->getOptimizerSettings();
	m_trainer = CNTKWrapper::CreateOptimizer(optimizer, m_QFunctionPtr, m_lossFunctionPtr);

	outputArguments(L"QFunctionPtr",m_QFunctionPtr);
	FunctionPtr foundPtr = m_networkFunctionPtr->FindByName(m_pNetworkDefinition->getOutputLayerName())->Output();
	outputArguments(L"Found QFunctionPtr", foundPtr);
}

INetwork* Network::clone() const
{
	Network* result = new Network(m_pNetworkDefinition);

	outputArguments(L"Original Network:",m_QFunctionPtr);

	result->m_networkFunctionPtr = m_networkFunctionPtr->Clone(ParameterCloningMethod::Clone);
	for each (auto input in result->m_networkFunctionPtr->Arguments())
	{
		wstring name = input.Name();
		if (m_pNetworkDefinition->isInputLayer(name))
			result->m_inputs.push_back(input);
		else //target
			result->m_targetVariable = (input);
	}

	result->m_QFunctionPtr = result->m_networkFunctionPtr->FindByName(
		m_pNetworkDefinition->getOutputLayerName())->Output();

	outputArguments(L"Cloned Network", result->m_QFunctionPtr);
	outputParameters(L"Original QNetwork", m_QFunctionPtr);
	outputParameters(L"Cloned QNetwork- parameters:", result->m_QFunctionPtr);

	result->m_lossFunctionPtr = m_networkFunctionPtr->FindByName(m_lossName)->Output();
	const OptimizerSettings* optimizer = m_pNetworkDefinition->getOptimizerSettings();
	result->m_trainer= CNTKWrapper::CreateOptimizer(optimizer, result->m_QFunctionPtr, result->m_lossFunctionPtr);

	size_t a= result->m_trainer->ParameterLearners().size();
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
		if (m_pNetworkDefinition->isInputLayer(inputVariable.Name()))
			m_inputs.push_back(inputVariable);
	}
	if (m_inputs.size() == 0)
		throw runtime_error("No input layer could be found in the network");
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
	for each (auto inputVariable in m_inputs)
	{
		arguments[inputVariable] = CNTK::Value::CreateBatch(inputVariable.Shape()
			, pMinibatch->getInputVector(), CNTK::DeviceDescriptor::CPUDevice());
	}
	//set target outputs
	arguments[m_targetVariable] = CNTK::Value::CreateBatch(m_targetVariable.Shape()
		, pMinibatch->getTargetOutputVector(), CNTK::DeviceDescriptor::CPUDevice());
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

void Network::get(const State* s, vector<double>& outputVector)
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

	vector<double> inputVector = vector<double>(m_pNetworkDefinition->getNumInputStateVars());
	for (size_t i = 0; i < m_pNetworkDefinition->getNumInputStateVars(); i++)
		inputVector[i] = s->get((int)m_pNetworkDefinition->getInputStateVar((int) i));

	//THIS WON'T WORK IF THERE IS MORE THAN 1 INPUT VARIABLE
	//DIFFERENT INPUT VECTORS SHOULD BE USED
	for each(auto inputVariable in m_inputs)
	{
		wstring uid1 = m_networkFunctionPtr->Arguments()[0].Uid();
		wstring asText= m_networkFunctionPtr->Arguments()[0].AsString();
		wstring asText2 = inputVariable.AsString();
		wstring uid2 = inputVariable.Uid();
		inputs[inputVariable] = CNTK::Value::CreateBatch(inputVariable.Shape()
			, inputVector, CNTK::DeviceDescriptor::CPUDevice());
	}
	size_t numargs = m_QFunctionPtr->Arguments().size();
	wstring argname;
	for (int i= 0; i<numargs; i++)
		argname = m_QFunctionPtr->Arguments()[0].Name();
	m_QFunctionPtr->Evaluate(inputs, outputs, CNTK::DeviceDescriptor::CPUDevice());

	outputValue = outputs[m_QFunctionPtr];

	CNTK::NDShape outputShape = m_QFunctionPtr->Output().Shape().AppendShape({ 1
		, outputVector.size() / m_QFunctionPtr->Output().Shape().TotalSize() });

	CNTK::NDArrayViewPtr cpuArrayOutput = CNTK::MakeSharedObject<CNTK::NDArrayView>(outputShape
		, outputVector, false);
	cpuArrayOutput->CopyFrom(*outputValue->Data());
}

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
}

#endif // _WIN64