#include "stdafx.h"

#ifdef _WIN64



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

void CNetwork::train(std::unordered_map<std::string, std::vector<double>&>& inputDataMap, std::vector<double>& targetOutputData)
{
	NDShape outputShape = getOutputsFunctionPtr().at(0)->Output().Shape();
	ValuePtr outputSequence = CNTK::Value::CreateBatch(outputShape, targetOutputData, CNTK::DeviceDescriptor::CPUDevice());

	std::unordered_map<CNTK::Variable, CNTK::ValuePtr> arguments = std::unordered_map<CNTK::Variable, CNTK::ValuePtr>();
	for each (auto item in m_inputs)
	{
		//only use inputs, which are actually needed/used in the model
		if (item->getIsUsed())
			arguments[item->getInputVariable()] = CNTK::Value::CreateBatch(item->getInputVariable().Shape(), inputDataMap.at(m_inputs[0]->getId()), CNTK::DeviceDescriptor::CPUDevice());
	}

	arguments[m_targetOutput] = outputSequence;

	m_trainer->TrainMinibatch(arguments, DeviceDescriptor::CPUDevice());
}

void CNetwork::predict(std::unordered_map<std::string, std::vector<double>&>& inputDataMap, std::vector<double>& predictionData)
{
	FunctionPtr outputPtr = getOutputsFunctionPtr().at(0);
	ValuePtr outputValue;

	std::unordered_map<CNTK::Variable, CNTK::ValuePtr> outputs = { { outputPtr->Output(), outputValue } };

	std::unordered_map<CNTK::Variable, CNTK::ValuePtr> inputs = std::unordered_map<CNTK::Variable, CNTK::ValuePtr>();
	for each (auto item in m_inputs)
	{
		//only use inputs, which are actually needed/used in the model
		if (item->getIsUsed())
		{
			inputs[item->getInputVariable()] = CNTK::Value::CreateBatch(item->getInputVariable().Shape(), inputDataMap.at(item->getId()), CNTK::DeviceDescriptor::CPUDevice());
		}
	}

	outputPtr->Evaluate(inputs, outputs, CNTK::DeviceDescriptor::CPUDevice());

	outputValue = outputs[outputPtr];

	if (predictionData.size() % outputPtr->Output().Shape().TotalSize())
	{
		throw std::runtime_error("predictionData does not have the right size.");
	}

	CNTK::NDShape outputShape = outputPtr->Output().Shape().AppendShape({ 1, predictionData.size() / outputPtr->Output().Shape().TotalSize() });

	CNTK::NDArrayViewPtr cpuArrayOutput = CNTK::MakeSharedObject<CNTK::NDArrayView>(outputShape, predictionData, false);
	cpuArrayOutput->CopyFrom(*outputValue->Data());
}

#include <iostream>
CNetwork* CNetwork::cloneNonTrainable() const
{
	CNetwork* result = new CNetwork();
	result->m_trainer = nullptr;
	result->m_lossFunctionPtr = nullptr;

	result->m_networkFunctionPtr = m_networkFunctionPtr->Clone(CNTK::ParameterCloningMethod::Clone);

	for each (CNTK::Variable var in result->m_networkFunctionPtr->Outputs())
	{
		if (var.Name() != L"Loss" && var.Name() != L"loss")
			result->m_outputsFunctionPtr.push_back(var);
	}

	for each (auto initem in m_inputs)
	{
		for each (auto item in result->m_networkFunctionPtr->Arguments())
		{
			if (CNTKWrapper::Internal::wstring2string(item.Name()) == initem->getId())
			{
				auto value = new CInputData(CNTKWrapper::Internal::wstring2string(item.Name()), item);
				value->setIsUsed(initem->getIsUsed());
				//TODO: set shape
				result->m_inputs.push_back(value);
				break;
			}
		}
	}



	return result;
}

#endif // _WIN64