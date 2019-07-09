#pragma once


class INetwork;
class IDeepNetwork;
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
		typedef IDeepNetwork* (CNTK_WRAPPER_DLL_API* getDiscreteQFunctionNetworkDll)(size_t numStateVariables, size_t totalNumActionSteps, string layers, DeepLearner learner, bool normalize);
		typedef IDeepNetwork* (CNTK_WRAPPER_DLL_API* getContinuousQFunctionNetworkDll)(size_t numStateVariables, size_t numActionVariables, string layers, DeepLearner learner, bool normalize);
		typedef IDeepNetwork* (CNTK_WRAPPER_DLL_API* getVFunctionNetworkDll)(size_t numStateVariables, string layers, DeepLearner learner, bool normalize);
		typedef IDeepNetwork* (CNTK_WRAPPER_DLL_API* getDeterministicPolicyNetworkDll)(size_t numStateVariables, string layers, DeepLearner learner, bool normalize);
		static getDiscreteQFunctionNetworkDll getDiscreteQFunctionNetwork;
		static getContinuousQFunctionNetworkDll getContinuousQFunctionNetwork;
		static getVFunctionNetworkDll getVFunctionNetwork;
		static getDeterministicPolicyNetworkDll getDeterministicPolicyNetwork;

		static void Load();
		static void UnLoad();
		static void RegisterDependencies();
	};
}