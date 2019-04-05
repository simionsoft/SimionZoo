/*
	SimionZoo: A framework for online model-free Reinforcement Learning on continuous
	control problems

	Copyright (c) 2016 SimionSoft. https://github.com/simionsoft

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#include "CNTKLibrary.h"
#include "Network.h"
#include "CNTKWrapperInternals.h"
#include "OptimizerSetting.h"
#include "InputData.h"
#include "NetworkDefinition.h"

#include <iostream>
#include <stdexcept>

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

	for (auto input : result->m_networkFunctionPtr->Arguments())
	{
		wstring name = input.Name();
		if (m_pNetworkDefinition->getStateInputLayer() == name)
		{
			result->m_s = input;
			result->m_bInputStateUsed = true;
		}
		else if (m_pNetworkDefinition->getActionInputLayer() == name)
		{
			result->m_a = input;
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
		throw std::runtime_error("Incorrect target in CNTKWrapper::Network::initSoftUpdate");
	
	size_t numOnlineParams = m_FunctionPtr->Parameters().size();
	size_t numTargetParams = pTargetNetwork->m_FunctionPtr->Parameters().size();
	if (numOnlineParams!=numTargetParams)
		throw std::runtime_error("Missmatched number of parameters in CNTKWrapper::Network::initSoftUpdate");

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
		throw std::runtime_error("Incorrect target in CNTKWrapper::Network::softUpdate");

	size_t numOnlineParams = m_FunctionPtr->Parameters().size();
	size_t numTargetParams = pTargetNetwork->m_FunctionPtr->Parameters().size();
	if (numOnlineParams != numTargetParams)
		throw std::runtime_error("Missmatched number of parameters in CNTKWrapper::Network::softUpdate");

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
	for (auto inputVariable : m_FunctionPtr->Arguments())
	{
		//This avoids the "Target" layer to be added to the list
		if (m_pNetworkDefinition->getStateInputLayer() == inputVariable.Name())
		{
			m_s = inputVariable;
			m_bInputStateUsed = true;
		}
		else if (m_pNetworkDefinition->getActionInputLayer() == inputVariable.Name())
		{
			m_a = inputVariable;
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


void Network::train(IMinibatch* pMinibatch, const vector<double>& target)
{
	unordered_map<CNTK::Variable, CNTK::MinibatchData> arguments =
		unordered_map<CNTK::Variable, CNTK::MinibatchData>();
	//set inputs
	if (m_bInputStateUsed)
	{
		arguments[m_s] = CNTK::Value::CreateBatch(m_s.Shape()
			, pMinibatch->s(), CNTK::DeviceDescriptor::UseDefaultDevice());
	}
	if (m_bInputActionUsed)
	{
		arguments[m_a] = CNTK::Value::CreateBatch(m_a.Shape()
			, pMinibatch->a(), CNTK::DeviceDescriptor::UseDefaultDevice());
	}

	//set target outputs
	arguments[m_targetVariable] = CNTK::Value::CreateBatch(m_targetVariable.Shape()
		, target, CNTK::DeviceDescriptor::UseDefaultDevice());
	//train the network using the minibatch
	m_trainer->TrainMinibatch(arguments, DeviceDescriptor::UseDefaultDevice());
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

void Network::evaluate(const vector<double>& s, const vector<double>& a, vector<double>& output)
{
	ValuePtr outputValue;

	unordered_map<CNTK::Variable, CNTK::ValuePtr> outputs =	{ { m_FunctionPtr->Output(), outputValue } };

	unordered_map<CNTK::Variable, CNTK::ValuePtr> inputs = {};

	if (m_bInputStateUsed)
		inputs[m_s] = CNTK::Value::CreateBatch(m_s.Shape(), s, CNTK::DeviceDescriptor::UseDefaultDevice());

	vector<double> inputAction;
	if (m_bInputActionUsed)
		inputs[m_a] = CNTK::Value::CreateBatch(m_a.Shape(), a, CNTK::DeviceDescriptor::UseDefaultDevice());

	m_FunctionPtr->Evaluate(inputs, outputs, CNTK::DeviceDescriptor::UseDefaultDevice());

	outputValue = outputs[m_FunctionPtr];

	CNTK::NDShape outputShape = m_FunctionPtr->Output().Shape().AppendShape({ 1
		, output.size() / m_FunctionPtr->Output().Shape().TotalSize() });

	CNTK::NDArrayViewPtr cpuArrayOutput = CNTK::MakeSharedObject<CNTK::NDArrayView>(outputShape
		, output, false);
	cpuArrayOutput->CopyFrom(*outputValue->Data());
}

vector<double>& Network::evaluate(const State* s, const Action* a)
{
	ValuePtr outputValue;

	unordered_map<CNTK::Variable, CNTK::ValuePtr> outputs =	{ { m_FunctionPtr->Output(), outputValue } };

	unordered_map<CNTK::Variable, CNTK::ValuePtr> inputs = {};

	vector<double> inputState;
	if (m_bInputStateUsed)
	{
		stateToVector(s, inputState);
		inputs[m_s] = CNTK::Value::CreateBatch(m_s.Shape()
			, inputState, CNTK::DeviceDescriptor::UseDefaultDevice());
	}
	vector<double> inputAction;
	if (m_bInputActionUsed)
	{
		actionToVector(a, inputAction);
		inputs[m_a] = CNTK::Value::CreateBatch(m_a.Shape()
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

void Network::gradientWrtAction(const vector<double>& s, const vector<double>& a, vector<double>& outputGradient)
{
	unordered_map<Variable, ValuePtr> arguments = {};
	unordered_map<Variable, ValuePtr> gradients = {};

	if (!m_bInputStateUsed || !m_bInputActionUsed)
		throw std::runtime_error("Can only use gradient() with f(s,a)-form functions");

	arguments[m_s] = CNTK::Value::CreateBatch(m_s.Shape()
		, s, CNTK::DeviceDescriptor::UseDefaultDevice());

	arguments[m_a] = CNTK::Value::CreateBatch(m_a.Shape()
		, a, CNTK::DeviceDescriptor::UseDefaultDevice());

	gradients[m_a] = nullptr;

	m_FunctionPtr->Gradients(arguments, gradients);

	//copy gradient to cpu vector
	ValuePtr gradient = gradients[m_a];
	if (gradient->Shape().TotalSize() != outputGradient.size())
		throw std::runtime_error("Missmatched length for output vector in gradients()");

	NDArrayViewPtr qParameterGradientCpuArrayView =
		MakeSharedObject<NDArrayView>(gradient->Shape(), outputGradient, false);
	qParameterGradientCpuArrayView->CopyFrom(*gradient->Data());
}

void Network::applyGradient(IMinibatch* pMinibatch, const vector<double>& target)
{
	//Similar to the actual training function in https://github.com/Microsoft/CNTK/blob/94e6582d2f63ce3bb048b9da01679abeacda877f/Source/CNTKv2LibraryDll/Trainer.cpp#L193
	//but with a different root value (taken from the minibatch) that, in the case of DDPG, should be -dQ(s,a)/da
	//Forward pass
	unordered_map<Variable, ValuePtr> arguments = {};
	unordered_map<Variable, ValuePtr> output = {};
	if (m_bInputStateUsed)
	{
		arguments[m_s] = CNTK::Value::CreateBatch(m_s.Shape()
			, pMinibatch->s(), CNTK::DeviceDescriptor::UseDefaultDevice());
	}
	if (m_bInputActionUsed)
	{
		arguments[m_a] = CNTK::Value::CreateBatch(m_a.Shape()
			, pMinibatch->a(), CNTK::DeviceDescriptor::UseDefaultDevice());
	}
	output[m_FunctionPtr] = nullptr;
	auto backPropState = m_FunctionPtr->Forward(arguments, output, DeviceDescriptor::UseDefaultDevice(), { m_FunctionPtr });

	//Backward pass
	vector<double> RootValue = vector<double>(pMinibatch->outputSize()*pMinibatch->size());
	auto RootGradientValue = MakeSharedObject<Value>(MakeSharedObject<NDArrayView>(
		m_FunctionPtr->Output().Shape().AppendShape({ 1, RootValue.size() / m_FunctionPtr->Output().Shape().TotalSize() })
		, RootValue, false));

	for (int i = 0; i < pMinibatch->outputSize()*pMinibatch->size(); i++)
		RootValue[i] = target[i];

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