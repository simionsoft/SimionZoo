#pragma once

class IDiscreteQFunctionNetwork;
class IContinuousQFunctionNetwork;
class IVFunctionNetwork;
class IDeterministicPolicyNetwork;

class INetwork;
class INetworkDefinition;

#include "parameters.h"

#include <string>
using namespace std;

namespace tinyxml2 { class XMLElement; }

#if defined(_WIN64)
	#define CNTK_WRAPPER_DLL_API __stdcall
#else
	#define CNTK_WRAPPER_DLL_API
#endif

namespace CNTK
{
	class WrapperClient
	{
	public:

		typedef INetworkDefinition* (CNTK_WRAPPER_DLL_API* getNetworkDefinitionDLL)(tinyxml2::XMLElement* pNode);
		typedef void(CNTK_WRAPPER_DLL_API* setDeviceDLL)(bool useGPU);
		static getNetworkDefinitionDLL getNetworkDefinition;
		static setDeviceDLL setDevice;

		//new interfaces
		typedef IDiscreteQFunctionNetwork* (CNTK_WRAPPER_DLL_API* getDiscreteQFunctionNetworkDll)(vector<string> inputStateVariables, size_t numActionSteps
			, string networkLayersDefinition, string learnerDefinition, bool useNormalization);
		typedef IContinuousQFunctionNetwork* (CNTK_WRAPPER_DLL_API* getContinuousQFunctionNetworkDll)(vector<string> inputStateVariables, vector<string> inputActionVariables
			, string networkLayersDefinition, string learnerDefinition, bool useNormalization);
		typedef IVFunctionNetwork* (CNTK_WRAPPER_DLL_API* getVFunctionNetworkDll)(vector<string> inputStateVariables, string networkLayersDefinition
			, string learnerDefinition, bool useNormalization);
		typedef IDeterministicPolicyNetwork* (CNTK_WRAPPER_DLL_API* getDeterministicPolicyNetworkDll)(vector<string> inputStateVariables, string networkLayersDefinition
			, string learnerDefinition, bool useNormalization);
		static getDiscreteQFunctionNetworkDll getDiscreteQFunctionNetwork;
		static getContinuousQFunctionNetworkDll getContinuousQFunctionNetwork;
		static getVFunctionNetworkDll getVFunctionNetwork;
		static getDeterministicPolicyNetworkDll getDeterministicPolicyNetwork;

		static void Load();
		static void UnLoad();
		static void RegisterDependencies();
	};
}