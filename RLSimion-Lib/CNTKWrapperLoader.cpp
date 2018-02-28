#include "CNTKWrapperLoader.h"
#include <windows.h>

HMODULE hCNTKWrapperDLL= 0;

CNTKWrapperLoader::getProblemInstanceDLL CNTKWrapperLoader::getProblem= 0;

void CNTKWrapperLoader::Init()
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

void CNTKWrapperLoader::Close()
{
	if (hCNTKWrapperDLL)
		FreeLibrary(hCNTKWrapperDLL);
}
