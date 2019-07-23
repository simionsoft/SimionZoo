#pragma once


#ifdef BUILD_CNTK_WRAPPER
	//we are building the CNTK Wrapper
	#ifdef __linux__
		#define DLL_API
	#else
		#define DLL_API __declspec(dllexport)
	#endif
#else
	#define DLL_API
#endif

#include <vector>

class IDiscreteQFunctionNetwork;
class IContinuousQFunctionNetwork;
class IVFunctionNetwork;
class IDeterministicPolicyNetwork;

namespace CNTKWrapper
{
	extern "C" {DLL_API void setDevice(bool useGPU); }

	extern "C" {DLL_API IDiscreteQFunctionNetwork* getDiscreteQFunctionNetwork(vector<string> inputStateVariables, size_t numActionSteps
		, string networkLayersDefinition, string learnerDefinition, bool useNormalization); }
	extern "C" {DLL_API IContinuousQFunctionNetwork* getContinuousQFunctionNetwork(vector<string> inputStateVariables, vector<string> inputActionVariables
		, string networkLayersDefinition, string learnerDefinition, bool useNormalization); }
	extern "C" {DLL_API IVFunctionNetwork* getVFunctionNetwork(vector<string> inputStateVariables, string networkLayersDefinition, string learnerDefinition, bool useNormalization); }
	extern "C" {DLL_API IDeterministicPolicyNetwork* getDeterministicPolicyNetwork(vector<string> inputStateVariables, vector<string> outputActionVariables, string networkLayersDefinition, string learnerDefinition, bool useNormalization); }
}

