#pragma once
#include "state-action-function.h"

class INetwork;
class INetworkDefinition;
namespace tinyxml2 { class XMLElement; }

#if defined(_WIN32) && defined(_WIN64)
	#define CNTK_WRAPPER_DLL_API __stdcall
#else
	#define CNTK_WRAPPER_DLL_API
#endif

namespace CNTK
{
	class WrapperClient
	{
	public:

		typedef INetworkDefinition* (CNTK_WRAPPER_DLL_API *getNetworkDefinitionDLL)(tinyxml2::XMLElement* pNode);
		typedef void(CNTK_WRAPPER_DLL_API *setDeviceDLL)(bool useGPU);
		static getNetworkDefinitionDLL getNetworkDefinition;
		static setDeviceDLL setDevice;

		static void Load();
		static void UnLoad();
	};
}