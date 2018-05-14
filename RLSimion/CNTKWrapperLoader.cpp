#include "CNTKWrapperLoader.h"
#ifdef _WIN64

#include <windows.h>
#include "app.h"
#include "logger.h"


HMODULE hCNTKWrapperDLL= 0;

CNTKWrapperLoader::getNetworkDefinitionDLL CNTKWrapperLoader::getNetworkDefinition = 0;
CNTKWrapperLoader::setDeviceDLL CNTKWrapperLoader::setDevice = 0;


void CNTKWrapperLoader::Load()
{
	if (hCNTKWrapperDLL == 0)
	{
		//Set the number of CPU threads to "all"
		SimionApp::get()->setNumCPUCores(0);

		//Load the wrapper library
#ifdef _DEBUG
		hCNTKWrapperDLL = LoadLibrary(".\\..\\Debug\\CNTKWrapper.dll");
#else
		//In release mode, it could be in the /bin folder (remote execution) or in /bin/x64 (local execution)
		//We try both to be safe
		hCNTKWrapperDLL = LoadLibrary(".\\..\\bin\\CNTKWrapper.dll");
#endif
		if (hCNTKWrapperDLL == 0)
			Logger::logMessage(MessageType::Error, "Failed to load CNTKWrapper.dll");

		Logger::logMessage(MessageType::Info, "Loading CNTK library");

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


		//get the address of the interface functions
		getNetworkDefinition = (getNetworkDefinitionDLL)GetProcAddress(hCNTKWrapperDLL, "CNTKWrapper::getNetworkDefinition");
		if (getNetworkDefinition==0)
			Logger::logMessage(MessageType::Error, "Failed to get a pointer to CNTKWrapper:getNetworkDefinition()");

		setDevice = (setDeviceDLL)GetProcAddress(hCNTKWrapperDLL, "CNTKWrapper::setDevice");
		if (setDevice == 0)
			Logger::logMessage(MessageType::Error, "Failed to get a pointer to CNTKWrapper:setDevice()");
	}
}

void CNTKWrapperLoader::UnLoad()
{
	if (hCNTKWrapperDLL)
	{
		FreeLibrary(hCNTKWrapperDLL);
		hCNTKWrapperDLL = 0;
	}
}

#endif