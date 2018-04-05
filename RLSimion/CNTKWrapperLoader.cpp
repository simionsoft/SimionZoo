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
#ifdef _DEBUG
		hCNTKWrapperDLL = LoadLibrary(".\\..\\Debug\\x64\\CNTKWrapper.dll");
#else
		hCNTKWrapperDLL = LoadLibrary(".\\..\\bin\\CNTKWrapper.dll");
		if (hCNTKWrapperDLL == 0)
			hCNTKWrapperDLL = LoadLibrary(".\\..\\bin\\x64\\CNTKWrapper.dll");
		if (hCNTKWrapperDLL == 0)
			Logger::logMessage(MessageType::Error, "Failed to load CNTKWrapper.dll");

		Logger::logMessage(MessageType::Info, "Loading CNTK library");

		SimionApp::get()->registerInputFile("..\\bin\\x64\\CNTKWrapper.dll", "..\\bin\\CNTKWrapper.dll");
		SimionApp::get()->registerInputFile("..\\bin\\x64\\Cntk.Core-2.1.dll", "..\\bin\\Cntk.Core-2.1.dll");
		SimionApp::get()->registerInputFile("..\\bin\\x64\\Cntk.Math-2.1.dll", "..\\bin\\Cntk.Math-2.1.dll");
		SimionApp::get()->registerInputFile("..\\bin\\x64\\Cntk.PerformanceProfiler-2.1.dll", "..\\bin\\Cntk.PerformanceProfiler-2.1.dll");
		SimionApp::get()->registerInputFile("..\\bin\\x64\\libiomp5md.dll", "..\\bin\\libiomp5md.dll");
		SimionApp::get()->registerInputFile("..\\bin\\x64\\mkl_cntk_p.dll", "..\\bin\\mkl_cntk_p.dll");
#endif

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