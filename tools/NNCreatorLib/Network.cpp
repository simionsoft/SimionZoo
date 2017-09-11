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
