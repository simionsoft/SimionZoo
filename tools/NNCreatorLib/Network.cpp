#include "stdafx.h"
#include "Network.h"
#include "CNTKWrapper.h"
#include "OptimizerSetting.h"


CNetwork::CNetwork()
{
	m_outputsFunctionPtr = vector<CNTK::FunctionPtr>();
}


CNetwork::~CNetwork()
{
}

void CNetwork::buildNetworkFunctionPtr(const COptimizerSetting* optimizer)
{
	auto shape = m_outputsFunctionPtr.at(0)->Output().Shape();

	m_targetOutput = CNTK::InputVariable(shape, CNTK::DataType::Float, L"Target");

	//TODO: add support for different loss functions
	m_lossFunctionPtr = CNTK::SquaredError(m_outputsFunctionPtr.at(0), m_targetOutput, L"Loss");

	m_networkFunctionPtr = CNTK::Combine({ m_lossFunctionPtr, m_outputsFunctionPtr.at(0) }, L"Network");

	m_trainer = CNTKWrapper::CreateOptimizer(optimizer, m_outputsFunctionPtr.at(0), m_lossFunctionPtr);
}

void CNetwork::save(string fileName)
{
	if (m_networkFunctionPtr == nullptr)
		throw std::runtime_error("Network has not been built yet. Call buildNetworkFunctionPtr() before calling save().");
	m_networkFunctionPtr->Save(CNTKWrapper::Internal::string2wstring(fileName));
}

CNetwork CNetwork::load(string fileName, CNTK::DeviceDescriptor &device)
{
	CNetwork result = CNetwork();
	result.m_networkFunctionPtr = CNTK::Function::Load(CNTKWrapper::Internal::string2wstring(fileName), device);
	return result;
}

void CNetwork::train(std::vector<float>& inputData, std::vector<float>& targetOutputData, size_t numSamples)
{
	NDShape inputShape = m_inputs[0]->getInputVariable().Shape();//.AppendShape({ 1, numSamples });
	NDShape outputShape = getOutputsFunctionPtr().at(0)->Output().Shape();// .AppendShape({ 1, numSamples });

	ValuePtr inputSequence = CNTK::Value::CreateSequence(inputShape, inputData, CNTK::DeviceDescriptor::CPUDevice());
	ValuePtr outputSequence = CNTK::Value::CreateSequence(outputShape, targetOutputData, CNTK::DeviceDescriptor::CPUDevice());

	std::unordered_map<CNTK::Variable, CNTK::ValuePtr> map = { { m_inputs[0]->getInputVariable(), inputSequence },{ m_targetOutput, outputSequence } };

	m_trainer->TrainMinibatch(map, DeviceDescriptor::CPUDevice());
}

void CNetwork::predict(std::unordered_map<std::string, std::vector<float>&>& inputDataMap, std::vector<float>& predictionData, size_t numSamples)
{
	FunctionPtr outputPtr = getOutputsFunctionPtr().at(0);
	ValuePtr outputValue;

	std::unordered_map<CNTK::Variable, CNTK::ValuePtr> outputs = { { outputPtr->Output(), outputValue } };

	std::unordered_map<CNTK::Variable, CNTK::ValuePtr> inputs = std::unordered_map<CNTK::Variable, CNTK::ValuePtr>();
	for each (auto item in m_inputs)
	{
		/*NDShape inputShape = m_inputs[0]->getInputVariable().Shape().AppendShape({ 1, numSamples });
		auto inputData = inputDataMap.at(m_inputs[0]->getId());
		ValuePtr inputValue = MakeSharedObject<Value>(MakeSharedObject<NDArrayView>(inputShape, inputData, true));
		inputs[m_inputs[0]->getInputVariable()] = inputValue;*/
	}
	NDShape inputShape = m_inputs[0]->getInputVariable().Shape().AppendShape({ 1, numSamples });
	auto inputData = inputDataMap.at(m_inputs[0]->getId());
	ValuePtr inputValue = MakeSharedObject<Value>(MakeSharedObject<NDArrayView>(inputShape, inputData, true));
	inputs[m_inputs[0]->getInputVariable()] = inputValue;

	outputPtr->Evaluate(inputs, outputs, CNTK::DeviceDescriptor::CPUDevice());
	outputValue = outputs[outputPtr];

	CNTK::NDShape outputShape = outputPtr->Output().Shape().AppendShape({ 1, numSamples });

	if (predictionData.size() != outputShape.TotalSize())
	{
		throw std::runtime_error("predictionData does not have the right size.");
	}

	CNTK::NDArrayViewPtr cpuArrayOutput = CNTK::MakeSharedObject<CNTK::NDArrayView>(outputShape, predictionData, false);
	cpuArrayOutput->CopyFrom(*outputValue->Data());
}

void CNetwork::train(std::unordered_map<std::string, CNTK::ValuePtr> inputDataMap, CNTK::ValuePtr targetOutputData)
{
}

void CNetwork::predict(std::unordered_map<std::string, CNTK::ValuePtr> inputDataMap, std::vector<float>& predictionData)
{
	FunctionPtr outputPtr = getOutputsFunctionPtr().at(0);
	ValuePtr outputValue;

	std::unordered_map<CNTK::Variable, CNTK::ValuePtr> outputs = { { outputPtr->Output(), outputValue } };

	std::unordered_map<CNTK::Variable, CNTK::ValuePtr> inputs = std::unordered_map<CNTK::Variable, CNTK::ValuePtr>();
	for each (auto item in m_inputs)
	{
		inputs[item->getInputVariable()] = inputDataMap[item->getId()];
	}

	outputPtr->Evaluate(inputs, outputs, CNTK::DeviceDescriptor::CPUDevice());

	outputValue = outputs[outputPtr];

	auto outputShape = outputPtr->Output().Shape();
	outputShape = outputPtr->Output().Shape().AppendShape({ 1, predictionData.size() / outputShape.TotalSize() });

	if (predictionData.size() != outputShape.TotalSize())
	{
		throw std::runtime_error("predictionData does not have the right size.");
	}

	CNTK::NDArrayViewPtr cpuArrayOutput = CNTK::MakeSharedObject<CNTK::NDArrayView>(outputShape, predictionData, false);
	cpuArrayOutput->CopyFrom(*outputValue->Data());
}