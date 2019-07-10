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

#include "CNTKWrapper.h"
#include "Parameter.h"
#include "Link.h"
#include "ParameterValues.h"
#include "Chain.h"
#include "NetworkArchitecture.h"
#include "NetworkDefinition.h"
#include "InputData.h"
#include "OptimizerSetting.h"
#include "Chain.h"
#include "Exceptions.h"
#include "Network.h"

#include "../Common/named-var-set.h"

#define EXPORT comment(linker, "/EXPORT:" __FUNCTION__ "=" __FUNCDNAME__)

INetworkDefinition* CNTKWrapper::getNetworkDefinition(tinyxml2::XMLElement* pNode)
{
#pragma EXPORT
	size_t maxNumCPUThreads = std::thread::hardware_concurrency();
	CNTK::SetMaxNumCPUThreads(maxNumCPUThreads);

	return NetworkDefinition::getInstance(pNode);
}

void CNTKWrapper::setDevice(bool useGPU)
{
#pragma EXPORT
	if (useGPU)
		CNTK::DeviceDescriptor::TrySetDefaultDevice(CNTK::DeviceDescriptor::GPUDevice(0));
	else
		CNTK::DeviceDescriptor::TrySetDefaultDevice(CNTK::DeviceDescriptor::CPUDevice());
}

IDiscreteQFunctionNetwork* getDiscreteQFunctionNetwork(DeepNetworkDefinition* pNetworkDefinition)
{
	return new CntkDiscreteQFunctionNetwork(pNetworkDefinition);
}
IContinuousQFunctionNetwork* getContinuousQFunctionNetwork(DeepNetworkDefinition* pNetworkDefinition)
{
	return new CntkContinuousQFunctionNetwork(pNetworkDefinition);
}
IVFunctionNetwork* getVFunctionNetwork(DeepNetworkDefinition* pNetworkDefinition)
{
	return new CntkVFunctionNetwork(pNetworkDefinition);
}
IDeterministicPolicyNetwork* getDeterministicPolicyNetwork(DeepNetworkDefinition* pNetworkDefinition)
{
	return new CntkDeterministicPolicyNetwork(pNetworkDefinition);
}

