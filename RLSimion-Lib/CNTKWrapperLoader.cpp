#include "CNTKWrapperLoader.h"
#ifdef _WIN64

#include <windows.h>
#include "app-rlsimion.h"
#include "SimGod.h"

HMODULE hCNTKWrapperDLL= 0;

CNTKWrapperLoader::getProblemInstanceDLL CNTKWrapperLoader::getProblem= 0;


void CNTKWrapperLoader::Load()
{
	CSimionApp::get()->registerInputFile("..\\bin\\x64\\Cntk.Core-2.1.dll", "..\\bin\\Cntk.Core-2.1.dll");
	CSimionApp::get()->registerInputFile("..\\bin\\x64\\Cntk.Math-2.1.dll", "..\\bin\\Cntk.Math-2.1.dll");
	CSimionApp::get()->registerInputFile("..\\bin\\x64\\Cntk.PerformanceProfiler-2.1.dll", "..\\bin\\Cntk.PerformanceProfiler-2.1.dll");
	CSimionApp::get()->registerInputFile("..\\bin\\x64\\libiomp5md.dll", "..\\bin\\libiomp5md.dll");
	CSimionApp::get()->registerInputFile("..\\bin\\x64\\mkl_cntk_p.dll", "..\\bin\\mkl_cntk_p.dll");
	if (hCNTKWrapperDLL == 0)
	{
#ifdef _DEBUG
		hCNTKWrapperDLL = LoadLibrary(".\\..\\Debug\\x64\\CNTKWrapper.dll");
#else
		hCNTKWrapperDLL = LoadLibrary(".\\..\\bin\\x64\\CNTKWrapper.dll");
#endif
		if (hCNTKWrapperDLL)
		{
			getProblem = (getProblemInstanceDLL)GetProcAddress(hCNTKWrapperDLL, "CNTKWrapper::getProblemInstance");
		}
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