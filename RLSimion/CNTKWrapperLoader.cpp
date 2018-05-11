#include "CNTKWrapperLoader.h"
#ifdef _WIN64

#include <windows.h>
#include "app.h"
#include "logger.h"


HMODULE hCNTKWrapperDLL= 0;

CNTKWrapperLoader::getProblemInstanceDLL CNTKWrapperLoader::getProblem= 0;


void CNTKWrapperLoader::Load()
{
	if (hCNTKWrapperDLL == 0)
	{
		//Set the number of CPU threads to "all"
		SimionApp::get()->setNumCPUCores(0);

		//Load the wrapper library
#ifdef _DEBUG
		hCNTKWrapperDLL = LoadLibrary(".\\..\\Debug\\x64\\CNTKWrapper.dll");
#else
		//In release mode, it could be in the /bin folder (remote execution) or in /bin/x64 (local execution)
		//We try both to be safe
		hCNTKWrapperDLL = LoadLibrary(".\\..\\bin\\CNTKWrapper.dll");
		if (hCNTKWrapperDLL == 0)
			hCNTKWrapperDLL = LoadLibrary(".\\..\\bin\\x64\\CNTKWrapper.dll");
#endif
		if (hCNTKWrapperDLL == 0)
			Logger::logMessage(MessageType::Error, "Failed to load CNTKWrapper.dll");

		Logger::logMessage(MessageType::Info, "Loading CNTK library");

		//register dependencies
		SimionApp::get()->registerInputFile("..\\bin\\x64\\CNTKWrapper.dll", "..\\bin\\CNTKWrapper.dll");
		SimionApp::get()->registerInputFile("..\\bin\\x64\\libiomp5md.dll", "..\\bin\\libiomp5md.dll");
		SimionApp::get()->registerInputFile("..\\bin\\x64\\mklml.dll", "..\\bin\\mklml.dll");
		SimionApp::get()->registerInputFile("..\\bin\\x64\\mkldnn.dll", "..\\bin\\mkldnn.dll");

		SimionApp::get()->registerInputFile("..\\bin\\x64\\cudnn64_7.dll", "..\\bin\\cudnn64_7.dll");
#ifdef _DEBUG
		SimionApp::get()->registerInputFile("..\\bin\\x64\\Cntk.Core-2.5.1d.dll", "..\\bin\\Cntk.Core-2.5.1d.dll");
		SimionApp::get()->registerInputFile("..\\bin\\x64\\Cntk.Math-2.5.1d.dll", "..\\bin\\Cntk.Math-2.5.1d.dll");
#else
		SimionApp::get()->registerInputFile("..\\bin\\x64\\Cntk.Core-2.5.1.dll", "..\\bin\\Cntk.Core-2.5.1.dll");
		SimionApp::get()->registerInputFile("..\\bin\\x64\\Cntk.Math-2.5.1.dll", "..\\bin\\Cntk.Math-2.5.1.dll");
#endif


		//get the address of the interface function
		getProblem = (getProblemInstanceDLL)GetProcAddress(hCNTKWrapperDLL, "CNTKWrapper::getProblemInstance");
		if (getProblem==0)
			Logger::logMessage(MessageType::Error, "Failed to get a pointer to CNTKWrapper:getProblemInstance()");
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