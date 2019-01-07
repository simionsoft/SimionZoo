#include "CNTKWrapperClient.h"

//so far, we only include CNTK-related stuff if we are under Windows AND x64 architecture
#if defined(_WIN32) && defined(_WIN64)
	#include "../tools/CNTKWrapper/CNTKWrapper.h"
	#include "../tools/System/DynamicLib.h"
#endif

#include "app.h"
#include "logger.h"


namespace CNTK
{
#if defined(_WIN32) && defined(_WIN64)
	DynamicLib DynamicLibCNTK;
	//HMODULE hCNTKWrapperDLL = 0;
#endif

	int NumNetworkInstances = 0;
	WrapperClient::getNetworkDefinitionDLL WrapperClient::getNetworkDefinition = 0;
	WrapperClient::setDeviceDLL WrapperClient::setDevice = 0;

	//We want to be able to know the requirements even if we are running Badger on a Win-32 machine
	//so, the only thing we actually don't do on Win-32 is load the dll and retrieve the access point

	void WrapperClient::Load()
	{
#if defined(_WIN32) && defined(_WIN64)
		NumNetworkInstances++;

		if (!DynamicLibCNTK.IsLoaded())

		{
			//Set the number of CPU threads to "all"
			SimionApp::get()->setNumCPUCores(0);
			SimionApp::get()->setRequiredArchitecture("Win-64");


			//Load the wrapper library
			Logger::logMessage(MessageType::Info, "Loading CNTK library");

#ifdef _DEBUG
			DynamicLibCNTK.Load("./../Debug/CNTKWrapper.dll");
			//hCNTKWrapperDLL = LoadLibrary("./../Debug/CNTKWrapper.dll");
#else
			DynamicLibCNTK.Load("./../bin/CNTKWrapper.dll");
			//hCNTKWrapperDLL = LoadLibrary("./../bin/CNTKWrapper.dll");
#endif
			//if (hCNTKWrapperDLL == 0)
			if (!DynamicLibCNTK.IsLoaded())
				Logger::logMessage(MessageType::Error, "Failed to load CNTKWrapper.dll");

			//get the address of the interface functions
			getNetworkDefinition = (getNetworkDefinitionDLL)DynamicLibCNTK.GetFuncAddress("CNTKWrapper::getNetworkDefinition");
				//GetProcAddress(hCNTKWrapperDLL, "CNTKWrapper::getNetworkDefinition");
			if (getNetworkDefinition == 0)
				Logger::logMessage(MessageType::Error, "Failed to get a pointer to CNTKWrapper:getNetworkDefinition()");

			setDevice = (setDeviceDLL) DynamicLibCNTK.GetFuncAddress("CNTKWrapper::setDevice");
				//GetProcAddress(hCNTKWrapperDLL, "CNTKWrapper::setDevice");
			if (setDevice == 0)
				Logger::logMessage(MessageType::Error, "Failed to get a pointer to CNTKWrapper:setDevice()");
		}
#endif


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

	}

	void WrapperClient::UnLoad()
	{
#if defined(_WIN32) && defined(_WIN64)
		NumNetworkInstances--;
		if (NumNetworkInstances==0 && DynamicLibCNTK.IsLoaded())
		//if (hCNTKWrapperDLL && NumNetworkInstances==0)
		{
			DynamicLibCNTK.Unload();
			//FreeLibrary(hCNTKWrapperDLL);
			//hCNTKWrapperDLL = 0;
		}
#endif
	}
}