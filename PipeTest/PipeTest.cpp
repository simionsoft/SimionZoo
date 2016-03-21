// PipeTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <windows.h>

#define MESSAGE_SIZE 1024
int _tmain(int argc, _TCHAR* argv[])
{
	wchar_t w_pipename[512];
	swprintf_s(w_pipename, 512, TEXT("\\\\.\\pipe\\%s"), "prueba");

	HANDLE inputPipe = CreateNamedPipe(
		w_pipename,             // pipe name 
		PIPE_ACCESS_INBOUND,       // write access 
		PIPE_TYPE_MESSAGE |       // message type pipe 
		PIPE_WAIT,                // blocking mode 
		PIPE_UNLIMITED_INSTANCES, // max. instances  
		0,              // output buffer size 
		1024,              // input buffer size 
		NMPWAIT_USE_DEFAULT_WAIT, // client time-out 
		NULL);                    // default security attribute  

	char buffer[10000];
	DWORD numBytesRead = 0;
	bool bEnd = false;
	while (!bEnd)
	{
		ReadFile(inputPipe, buffer, MESSAGE_SIZE, &numBytesRead, 0);
		if (numBytesRead > 0)
		{
			printf("Message read:\n%s\n", buffer);
			bEnd = true;
		}
		else
			Sleep(500);
	}

	CloseHandle(inputPipe);
	return 0;
}

