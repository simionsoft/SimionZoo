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

#include "CNTKWrapperClient.h"

//so far, we only include CNTK-related stuff if we are under Windows-64 OR Linux architecture (so far, only Linux-64 is supported)
#if defined(__linux__) || defined(_WIN64)
	#include "../CNTKWrapper/CNTKWrapper.h"
	#include "../../tools/System/DynamicLib.h"

	#ifdef __linux__
		#define GET_NETWORK_DEFINITION_FUNC_NAME "getNetworkDefinition"
		#define SET_DEVICE_FUNC_NAME "setDevice"
		#ifdef _DEBUG
			#define CNTK_WRAPPER_LIB_PATH "./../debug/CNTKWrapper-linux.so"
		#else
			#define CNTK_WRAPPER_LIB_PATH "./../bin/CNTKWrapper-linux.so"
		#endif
	#else
		#define GET_NETWORK_DEFINITION_FUNC_NAME "CNTKWrapper::getNetworkDefinition"
		#define SET_DEVICE_FUNC_NAME "CNTKWrapper::setDevice"
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
	WrapperClient::getNetworkDefinitionDLL WrapperClient::getNetworkDefinition = 0;
	WrapperClient::setDeviceDLL WrapperClient::setDevice = 0;

	//We want to be able to know the requirements even if we are running Badger on a Win-32 machine
	//so, the only thing we actually don't do on Win-32 is load the dll and retrieve the access point

	void WrapperClient::Load()
	{
#if defined(__linux__) || defined(_WIN64)
		NumNetworkInstances++;

		if (!DynamicLibCNTK.IsLoaded())

		{
			//Set the number of CPU threads to "all"
			SimionApp::get()->setNumCPUCores(0);
#ifdef __linux__
			SimionApp::get()->setRequiredArchitecture("Linux-64");
#else
			SimionApp::get()->setRequiredArchitecture("Win-64");
#endif


			//Load the wrapper library
			Logger::logMessage(MessageType::Info, "Loading CNTK library");

			DynamicLibCNTK.Load(CNTK_WRAPPER_LIB_PATH);

			if (!DynamicLibCNTK.IsLoaded())
				Logger::logMessage(MessageType::Error, "Failed to load dynamic library: CNTKWrapper");

			//get the address of the interface functions
			getNetworkDefinition = (getNetworkDefinitionDLL)DynamicLibCNTK.GetFuncAddress(GET_NETWORK_DEFINITION_FUNC_NAME);

			if (getNetworkDefinition == 0)
				Logger::logMessage(MessageType::Error, "Failed to get a pointer to CNTKWrapper:getNetworkDefinition()");

			setDevice = (setDeviceDLL) DynamicLibCNTK.GetFuncAddress(SET_DEVICE_FUNC_NAME);

			if (setDevice == 0)
				Logger::logMessage(MessageType::Error, "Failed to get a pointer to CNTKWrapper:setDevice()");
		}
#endif

#ifdef __WIN64
		//register dependencies
		SimionApp::get()->registerInputFile("../bin/CNTKWrapper.dll");

		SimionApp::get()->registerInputFile("../bin/Cntk.Composite-2.5.1.dll");
		SimionApp::get()->registerInputFile("../bin/Cntk.Core-2.5.1.dll");
		SimionApp::get()->registerInputFile("../bin/Cntk.Math-2.5.1.dll");
		SimionApp::get()->registerInputFile("../bin/Cntk.PerformanceProfiler-2.5.1.dll");

		SimionApp::get()->registerInputFile("../bin/cublas64_90.dll");
		SimionApp::get()->registerInputFile("../bin/cudart64_90.dll");
		SimionApp::get()->registerInputFile("../bin/cudnn64_7.dll");

		SimionApp::get()->registerInputFile("../bin/libiomp5md.dll");
		SimionApp::get()->registerInputFile("../bin/mklml.dll");
		SimionApp::get()->registerInputFile("../bin/mkl_cntk_p.dll");
		SimionApp::get()->registerInputFile("../bin/mkldnn.dll");
		SimionApp::get()->registerInputFile("../bin/nvml.dll");
#endif
#ifdef __linux__
		SimionApp::get()->registerInputFile("../bin/CNTKWrapper-linux.so");

		SimionApp::get()->registerInputFile("../bin/cntk-linux/libCntk.Core-2.5.1.so", "../bin/libCntk.Core-2.5.1.so");
		SimionApp::get()->registerInputFile("../bin/cntk-linux/libCntk.Math-2.5.1.so", "../bin/libCntk.Math-2.5.1.so");
		SimionApp::get()->registerInputFile("../bin/cntk-linux/libCntk.PerformanceProfiler-2.5.1.so", "../bin/libCntk.PerformanceProfiler-2.5.1.so");
		SimionApp::get()->registerInputFile("../bin/cntk-linux/libmklml_intel.so", "../bin/libmklml_intel.so");
		SimionApp::get()->registerInputFile("../bin/cntk-linux/libiomp5.so", "../bin/libiomp5.so");
#endif
	}

	void WrapperClient::UnLoad()
	{
#if defined(__linux__) || defined(_WIN64)
		NumNetworkInstances--;
		if (NumNetworkInstances==0 && DynamicLibCNTK.IsLoaded())
		{
			DynamicLibCNTK.Unload();
		}
#endif
	}
}