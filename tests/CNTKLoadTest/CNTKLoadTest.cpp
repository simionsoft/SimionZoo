// CNTKLoadTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>

typedef bool(__stdcall* FNC_INT_PARAM)(int parameter);
FNC_INT_PARAM intFunc;


#define CNTK_HEADERONLY_DEFINITIONS
#include "../../packages/CNTK.CPUOnly.2.1.0/build/native/include/CNTKLibrary.h"
class CLink;
#include <vector>


//typedef int(__stdcall* FOO_FNC)(int);
//FOO_FNC foo;

//typedef HRESULT(CALLBACK* LPFNDLLFUNC1)(DWORD, UINT*);

typedef CNTK::FunctionPtr (__stdcall* InputLayerFNC)(const CLink* pLink, std::vector<const CLink*> dependencies, CNTK::DeviceDescriptor& device);
InputLayerFNC InputLayerA;
int main()
{
	HMODULE hLibrary= LoadLibrary(L".\\..\\..\\bin\\x64\\CNTKWrapper.dll");

	if (hLibrary)
	{
		//foo(12);
		InputLayerA = (InputLayerFNC) GetProcAddress(hLibrary, "InputLayer");
		FreeLibrary(hLibrary);
	}
    return 0;
}

