#include <stdio.h>
#include <string>
#include <thread>
#include "../WindowsUtils/NamedPipe.h"


#ifdef _DEBUG
#pragma comment(lib,"../../Debug/WindowsUtils.lib")
#else
#pragma comment(lib,"../../bin/WindowsUtils.lib")
#endif


void main(char** argv, int argc)
{
	char message[100] = "my message";
	char pipename1[100];

	char buffer[100];
	CNamedPipeServer server;

	bool b1= server.openUniqueNamedPipeServer("myPipe");
	strcpy_s(pipename1, 100, server.getPipeFullName());
	//bool b2= server.openUniqueNamedPipeServer("myPipe");
	//strcpy_s(pipename2, 100, server.getPipeFullName());

	printf("waiting for connection");
	bool bConnected= server.waitForClientConnection();

	if (bConnected)
	{
		printf("SERVER: Client connected\n");
		int numBytes= server.readToBuffer(buffer, strlen(message)+1);
		printf("SERVER: %d bytes read: %s\n", numBytes, buffer);
	}
	else
	{
		printf("SERVER: Client didn't connect. Failed\n");
	}
	printf("SERVER: shutting down");
	server.closeServer();
	std::this_thread::sleep_for(std::chrono::milliseconds(5000));
}