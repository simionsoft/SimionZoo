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

#include "cntk-wrapper-loader.h"

//so far, we only include CNTK-related stuff if we are under Windows-64 OR Linux architecture (so far, only Linux-64 is supported)
#if defined(__linux__) || defined(_WIN64)
	#include "../CNTKWrapper/cntk-wrapper.h"
	#include "../../tools/System/DynamicLib.h"

	#ifdef __linux__
		#define SET_DEVICE_FUNC_NAME "setDevice"
		#define GET_DISCRETE_Q_FUNCTION_NETWORK_FUNC_NAME "getDiscreteQFunctionNetwork"
		#define GET_CONTINUOUS_Q_FUNCTION_NETWORK_FUNC_NAME "getContinuousQFunctionNetwork"
		#define GET_V_FUNCTION_NETWORK_FUNC_NAME "getVFunctionNetwork"
		#define GET_DETERMINISTIC_POLICY_NETWORK_FUNC_NAME "getDeterministicPolicyNetwork"
		#ifdef _DEBUG
			#define CNTK_WRAPPER_LIB_PATH "./../debug/CNTKWrapper-linux.so"
		#else
			#define CNTK_WRAPPER_LIB_PATH "./../bin/CNTKWrapper-linux.so"
		#endif
	#else
		#define SET_DEVICE_FUNC_NAME "CNTKWrapper::setDevice"
		#define GET_DISCRETE_Q_FUNCTION_NETWORK_FUNC_NAME "CNTKWrapper::getDiscreteQFunctionNetwork"
		#define GET_CONTINUOUS_Q_FUNCTION_NETWORK_FUNC_NAME "CNTKWrapper::getContinuousQFunctionNetwork"
		#define GET_V_FUNCTION_NETWORK_FUNC_NAME "CNTKWrapper::getVFunctionNetwork"
		#define GET_DETERMINISTIC_POLICY_NETWORK_FUNC_NAME "CNTKWrapper::getDeterministicPolicyNetwork"

		#ifdef _DEBUG
			#define CNTK_WRAPPER_LIB_PATH "./../debug/CNTKWrapper.dll"
		#else
			#define CNTK_WRAPPER_LIB_PATH "./../bin/CNTKWrapper.dll"
		#endif
	#endif
#endif

#include "app.h"
#include "logger.h"


namespace CNTK
{
#if defined(__linux__) || defined(_WIN64)
	DynamicLib DynamicLibCNTK;
#endif

	int NumNetworkInstances = 0;

	WrapperLoader::setDeviceDLL WrapperLoader::setDevice = nullptr;

	WrapperLoader::getDiscreteQFunctionNetworkDll WrapperLoader::getDiscreteQFunctionNetwork = nullptr;
	WrapperLoader::getContinuousQFunctionNetworkDll WrapperLoader::getContinuousQFunctionNetwork = nullptr;
	WrapperLoader::getVFunctionNetworkDll WrapperLoader::getVFunctionNetwork = nullptr;
	WrapperLoader::getDeterministicPolicyNetworkDll WrapperLoader::getDeterministicPolicyNetwork = nullptr;

	/// <summary>
	/// This loads the Cntk libraries. Only usable by x64 versions. We want to be able to know the requirements even
	/// if we are running Badger on a Win-32 machine so, the only thing we actually don't do on Win-32 is load the dll
	/// and retrieve the access point. Otherwise, it adds all the target-specific requirements with disregard to the
	/// platform on which the executable is being run
	/// </summary>
	void WrapperLoader::Load()
	{
#if defined(__linux__) || defined(_WIN64)
		NumNetworkInstances++;

		if (!DynamicLibCNTK.IsLoaded())
		{
			//Load the wrapper library
			Logger::logMessage(MessageType::Info, "Loading CNTK library");

			DynamicLibCNTK.Load(CNTK_WRAPPER_LIB_PATH);

			if (!DynamicLibCNTK.IsLoaded())
				Logger::logMessage(MessageType::Error, "Failed to load dynamic library: CNTKWrapper");

			//get the address of the interface functions
			getDiscreteQFunctionNetwork = (getDiscreteQFunctionNetworkDll)DynamicLibCNTK.GetFuncAddress(GET_DISCRETE_Q_FUNCTION_NETWORK_FUNC_NAME);
			getContinuousQFunctionNetwork = (getContinuousQFunctionNetworkDll)DynamicLibCNTK.GetFuncAddress(GET_CONTINUOUS_Q_FUNCTION_NETWORK_FUNC_NAME);
			getVFunctionNetwork = (getVFunctionNetworkDll)DynamicLibCNTK.GetFuncAddress(GET_V_FUNCTION_NETWORK_FUNC_NAME);
			getDeterministicPolicyNetwork = (getDeterministicPolicyNetworkDll)DynamicLibCNTK.GetFuncAddress(GET_DETERMINISTIC_POLICY_NETWORK_FUNC_NAME);

			if (getDiscreteQFunctionNetwork == nullptr || getContinuousQFunctionNetwork == nullptr
				|| getVFunctionNetwork == nullptr || getDeterministicPolicyNetwork == nullptr)
				Logger::logMessage(MessageType::Error, "Failed to get a pointer to an interface of the CNTKWrapper");

			setDevice = (setDeviceDLL)DynamicLibCNTK.GetFuncAddress(SET_DEVICE_FUNC_NAME);

			if (setDevice == 0)
				Logger::logMessage(MessageType::Error, "Failed to get a pointer to CNTKWrapper:setDevice()");

			setDevice(SimionApp::get()->getPreferredDevice());
		}
#endif
	}

	/// <summary>
	/// Unloads Cntk libraries if all instances of the client have been destroyed. We need to keep track of them
	/// because in an experiment we may be using more than one instance of Neural Network and we only want to unload
	/// the libraries once al Neural Networks have been destroyed
	/// </summary>
	void WrapperLoader::UnLoad()
	{
#if defined(__linux__) || defined(_WIN64)
		NumNetworkInstances--;
		if (NumNetworkInstances == 0 && DynamicLibCNTK.IsLoaded())
		{
			DynamicLibCNTK.Unload();
		}
#endif
	}


	/// <summary>
	/// Sets the requirements linked to the Cntk libraries: it registers the library dependencies and sets the number of CPU cores to "all".
	/// It's separated from the loading function so that dependencies can be calculated without actually loading Cntk libraries
	/// </summary>
	void WrapperLoader::SetRequirements()
	{
		//Set the number of CPU threads to "all"
		SimionApp::get()->setNumCPUCores(0);

		SimionApp::get()->registerTargetPlatformInputFile("Win-64", "../bin/CNTKWrapper.dll");

		SimionApp::get()->registerTargetPlatformInputFile("Win-64", "../bin/Cntk.Composite-2.5.1.dll");
		SimionApp::get()->registerTargetPlatformInputFile("Win-64", "../bin/Cntk.Core-2.5.1.dll");
		SimionApp::get()->registerTargetPlatformInputFile("Win-64", "../bin/Cntk.Math-2.5.1.dll");
		SimionApp::get()->registerTargetPlatformInputFile("Win-64", "../bin/Cntk.PerformanceProfiler-2.5.1.dll");

		SimionApp::get()->registerTargetPlatformInputFile("Win-64", "../bin/cublas64_90.dll");
		SimionApp::get()->registerTargetPlatformInputFile("Win-64", "../bin/cudart64_90.dll");
		SimionApp::get()->registerTargetPlatformInputFile("Win-64", "../bin/cudnn64_7.dll");
		SimionApp::get()->registerTargetPlatformInputFile("Win-64", "../bin/curand64_90.dll");
		SimionApp::get()->registerTargetPlatformInputFile("Win-64", "../bin/cusparse64_90.dll");

		SimionApp::get()->registerTargetPlatformInputFile("Win-64", "../bin/libiomp5md.dll");
		SimionApp::get()->registerTargetPlatformInputFile("Win-64", "../bin/mklml.dll");
		SimionApp::get()->registerTargetPlatformInputFile("Win-64", "../bin/mkldnn.dll");
		SimionApp::get()->registerTargetPlatformInputFile("Win-64", "../bin/nvml.dll");
		//SimionApp::get()->registerTargetPlatformInputFile("Win-64", "../bin/vcomp140.dll");

		SimionApp::get()->registerTargetPlatformInputFile("Linux-64", "../bin/CNTKWrapper-linux.so");

		SimionApp::get()->registerTargetPlatformInputFile("Linux-64", "../bin/cntk-linux/libCntk.Core-2.5.1.so", "../bin/libCntk.Core-2.5.1.so");
		SimionApp::get()->registerTargetPlatformInputFile("Linux-64", "../bin/cntk-linux/libCntk.Math-2.5.1.so", "../bin/libCntk.Math-2.5.1.so");
		SimionApp::get()->registerTargetPlatformInputFile("Linux-64", "../bin/cntk-linux/libCntk.PerformanceProfiler-2.5.1.so", "../bin/libCntk.PerformanceProfiler-2.5.1.so");
		SimionApp::get()->registerTargetPlatformInputFile("Linux-64", "../bin/cntk-linux/libmklml_intel.so", "../bin/libmklml_intel.so");
		SimionApp::get()->registerTargetPlatformInputFile("Linux-64", "../bin/cntk-linux/libiomp5.so", "../bin/libiomp5.so");
		SimionApp::get()->registerTargetPlatformInputFile("Linux-64", "../bin/cntk-linux/libmpi.so.12", "../bin/libmpi.so.12");
		SimionApp::get()->registerTargetPlatformInputFile("Linux-64", "../bin/cntk-linux/libmpi_cxx.so.1", "../bin/libmpi_cxx.so.1");
		SimionApp::get()->registerTargetPlatformInputFile("Linux-64", "../bin/cntk-linux/libopen-pal.so.13", "../bin/libopen-pal.so.13");
		SimionApp::get()->registerTargetPlatformInputFile("Linux-64", "../bin/cntk-linux/libopen-rte.so.12", "../bin/libopen-rte.so.12");
	}
}