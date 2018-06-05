#include "CNTKWrapperClient.h"
#include "../tools/CNTKWrapper/CNTKWrapper.h"

#include <windows.h>
#undef min
#undef max

#include "app.h"
#include "logger.h"


namespace CNTK
{

	HMODULE hCNTKWrapperDLL = 0;

	WrapperClient::getNetworkDefinitionDLL WrapperClient::getNetworkDefinition = 0;
	WrapperClient::setDeviceDLL WrapperClient::setDevice = 0;

	//We want to be able to know the requirements even if we are running Badger on a Win-32 machine
	//so, the only thing we actually don't do on Win-32 is load the dll and retrieve the access point

	void WrapperClient::Load()
	{

		if (hCNTKWrapperDLL == 0)
		{
			//Set the number of CPU threads to "all"
			SimionApp::get()->setNumCPUCores(0);
			SimionApp::get()->setRequiredArchitecture("Win-64");

#ifdef _WIN64
			//Load the wrapper library
			Logger::logMessage(MessageType::Info, "Loading CNTK library");

#ifdef _DEBUG
			hCNTKWrapperDLL = LoadLibrary(".\\..\\Debug\\CNTKWrapper.dll");
#else
			hCNTKWrapperDLL = LoadLibrary(".\\..\\bin\\CNTKWrapper.dll");
#endif
			if (hCNTKWrapperDLL == 0)
				Logger::logMessage(MessageType::Error, "Failed to load CNTKWrapper.dll");

			//get the address of the interface functions
			getNetworkDefinition = (getNetworkDefinitionDLL)GetProcAddress(hCNTKWrapperDLL, "CNTKWrapper::getNetworkDefinition");
			if (getNetworkDefinition == 0)
				Logger::logMessage(MessageType::Error, "Failed to get a pointer to CNTKWrapper:getNetworkDefinition()");

			setDevice = (setDeviceDLL)GetProcAddress(hCNTKWrapperDLL, "CNTKWrapper::setDevice");
			if (setDevice == 0)
				Logger::logMessage(MessageType::Error, "Failed to get a pointer to CNTKWrapper:setDevice()");

#endif


			//register dependencies
			SimionApp::get()->registerInputFile("..\\bin\\CNTKWrapper.dll");

			SimionApp::get()->registerInputFile("..\\bin\\Cntk.Composite-2.5.1.dll");
			SimionApp::get()->registerInputFile("..\\bin\\Cntk.Core-2.5.1.dll");
			SimionApp::get()->registerInputFile("..\\bin\\Cntk.Math-2.5.1.dll");
			SimionApp::get()->registerInputFile("..\\bin\\Cntk.PerformanceProfiler-2.5.1.dll");

			SimionApp::get()->registerInputFile("..\\bin\\cublas64_90.dll");
			SimionApp::get()->registerInputFile("..\\bin\\cudart64_90.dll");
			SimionApp::get()->registerInputFile("..\\bin\\cudnn64_7.dll");

			SimionApp::get()->registerInputFile("..\\bin\\libiomp5md.dll");
			SimionApp::get()->registerInputFile("..\\bin\\mklml.dll");
			SimionApp::get()->registerInputFile("..\\bin\\mkl_cntk_p.dll");
			SimionApp::get()->registerInputFile("..\\bin\\mkldnn.dll");
			SimionApp::get()->registerInputFile("..\\bin\\nvml.dll");
		}
	}

	void WrapperClient::UnLoad()
	{
#ifdef _WIN64
		if (hCNTKWrapperDLL)
		{
			FreeLibrary(hCNTKWrapperDLL);
			hCNTKWrapperDLL = 0;
		}
#endif
	}
}