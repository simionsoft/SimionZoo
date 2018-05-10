
#ifdef _WIN64

#include "../CNTKWrapper/CNTKWrapper.h"
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


#define EXPORT comment(linker, "/EXPORT:" __FUNCTION__ "=" __FUNCDNAME__)

INetworkDefinition* CNTKWrapper::getProblemInstance(tinyxml2::XMLElement* pNode)
{
#pragma EXPORT
	size_t maxNumCPUThreads = std::thread::hardware_concurrency();
	CNTK::SetMaxNumCPUThreads(maxNumCPUThreads);
	return NetworkDefinition::getInstance(pNode);
}



#endif // _WIN64