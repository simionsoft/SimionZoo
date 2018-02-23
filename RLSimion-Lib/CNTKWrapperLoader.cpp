#include "CNTKWrapperLoader.h"
#include <windows.h>

HMODULE hCNTKWrapperDLL= 0;

CNTKWrapper::getProblemInstanceDLL getProblem= 0;


void CNTKWrapper::Init()
{
	if (hCNTKWrapperDLL)
	{
		getProblem= (getProblemInstanceDLL)GetProcAddress(hCNTKWrapperDLL, "CNTKWrapper::getProblemInstance");
	}
}

void CNTKWrapper::Close()
{
	if (hCNTKWrapperDLL)
		FreeLibrary(hCNTKWrapperDLL);
}