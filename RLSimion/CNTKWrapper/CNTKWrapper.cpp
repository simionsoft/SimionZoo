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
