#include "stdafx.h"

#ifdef _WIN64


#include "CNTKLibrary.h"
#include "Network.h"
#include "CNTKWrapperInternals.h"
#include "OptimizerSetting.h"
#include "InputData.h"
#include "Problem.h"

Network::Network()
{
	m_outputsFunctionPtr = vector<CNTK::FunctionPtr>();
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
	const OptimizerSettings* optimizer = m_pParent->getOptimizerSettings();
	auto shape = m_outputsFunctionPtr.at(0)->Output().Shape();

	m_targetOutput = CNTK::InputVariable(shape, CNTK::DataType::Double, L"Target");

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

Network Network::load(string fileName, CNTK::DeviceDescriptor &device)
{
	Network result = Network();
	result.m_networkFunctionPtr = CNTK::Function::Load(CNTKWrapper::Internal::string2wstring(fileName), device);
	return result;
}

void Network::train(std::unordered_map<std::string, std::vector<double>&>& inputDataMap, std::vector<double>& targetOutputData)
{
	NDShape outputShape = getOutputsFunctionPtr().at(0)->Output().Shape();
	ValuePtr outputSequence = CNTK::Value::CreateBatch(outputShape, targetOutputData, CNTK::DeviceDescriptor::CPUDevice());

	std::unordered_map<CNTK::Variable, CNTK::ValuePtr> arguments = std::unordered_map<CNTK::Variable, CNTK::ValuePtr>();
	for each (auto item in m_inputs)
	{
		//only use inputs, which are actually needed/used in the model
		if (item->getIsUsed())
			arguments[item->getInputVariable()] = 
				CNTK::Value::CreateBatch(item->getInputVariable().Shape()
				, inputDataMap.at(item->getId()), CNTK::DeviceDescriptor::CPUDevice());
	}

	arguments[m_targetOutput] = outputSequence;

	m_trainer->TrainMinibatch(arguments, DeviceDescriptor::CPUDevice());
}

void Network::gradients(std::unordered_map<std::string, std::vector<double>&>& inputDataMap
	, std::vector<double>& targetOutputData, std::unordered_map<CNTK::Variable
	, CNTK::ValuePtr>& gradients)
{
	FunctionPtr outputPtr = getOutputsFunctionPtr().at(0);
	ValuePtr outputValue;
	std::unordered_map<CNTK::Variable, CNTK::ValuePtr> outputs =
		{ { outputPtr->Output(), outputValue } ,{ m_lossFunctionPtr , nullptr } };
	std::unordered_map<CNTK::Variable, CNTK::ValuePtr> inputs =
		std::unordered_map<CNTK::Variable, CNTK::ValuePtr>();
	for each (auto item in m_inputs)
	{
		//only use inputs, which are actually needed/used in the model
		if (item->getIsUsed())
		{
			inputs[item->getInputVariable()] =
				CNTK::Value::CreateBatch(item->getInputVariable().Shape()
					, inputDataMap.at(item->getId()), CNTK::DeviceDescriptor::CPUDevice());
		}
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

void Network::gradients(std::unordered_map<std::string, std::vector<double>&>& inputDataMap
	, std::unordered_map<CNTK::Variable, CNTK::ValuePtr>& gradients)
{
	FunctionPtr outputPtr = getOutputsFunctionPtr().at(0);
	ValuePtr outputValue;

	std::unordered_map<CNTK::Variable, CNTK::ValuePtr> outputs =
		{ { outputPtr->Output(), outputValue } };
	std::unordered_map<CNTK::Variable, CNTK::ValuePtr> inputs =
		std::unordered_map<CNTK::Variable, CNTK::ValuePtr>();
	for each (auto item in m_inputs)
	{
		//only use inputs, which are actually needed/used in the model
		if (item->getIsUsed())
		{
				inputs[item->getInputVariable()] = 
					CNTK::Value::CreateBatch(item->getInputVariable().Shape()
						, inputDataMap.at(item->getId()), CNTK::DeviceDescriptor::CPUDevice());
		}
	}
	outputPtr->Gradients(inputs, gradients);
}

void Network::predict(std::unordered_map<std::string, std::vector<double>&>& inputDataMap
	, std::vector<double>& predictionData)
{
	FunctionPtr outputPtr = getOutputsFunctionPtr().at(0);
	ValuePtr outputValue;

	std::unordered_map<CNTK::Variable, CNTK::ValuePtr> outputs =
	{ { outputPtr->Output(), outputValue } };

	std::unordered_map<CNTK::Variable, CNTK::ValuePtr> inputs =
		std::unordered_map<CNTK::Variable, CNTK::ValuePtr>();
	for each (auto item in m_inputs)
	{
		//only use inputs, which are actually needed/used in the model
		if (item->getIsUsed())
		{
			inputs[item->getInputVariable()] =
				CNTK::Value::CreateBatch(item->getInputVariable().Shape()
					, inputDataMap.at(item->getId()), CNTK::DeviceDescriptor::CPUDevice());
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
INetwork* Network::clone() const
{
	Network* result = new Network();
	result->m_trainer = nullptr;
	result->m_lossFunctionPtr = nullptr;
	result->setParent(m_pParent);

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
				auto value = new InputData(CNTKWrapper::Internal::wstring2string(item.Name()), item);
				value->setIsUsed(initem->getIsUsed());
				//TODO: set shape
				result->m_inputs.push_back(value);
				break;
			}
		}
	}

	return result;
}

void Network::setParent(INetworkDefinition* pProblem)
{
	m_pParent = pProblem;
}

#endif // _WIN64