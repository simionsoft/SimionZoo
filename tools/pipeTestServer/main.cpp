#include <stdio.h>
#include <string>
#include "../NamedPipe/NamedPipe.h"

#pragma comment(lib,"../../Debug/NamedPipe.lib")


void main(char** argv, int argc)
{
	char message[100] = "my message";
	char buffer[100];
	CNamedPipeServer server;

	server.openNamedPipeServer("myPipe");

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
}