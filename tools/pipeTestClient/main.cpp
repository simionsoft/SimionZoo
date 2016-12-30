#include <stdio.h>
#include <string>
#include <thread>
#include "../NamedPipe/NamedPipe.h"

#pragma comment(lib,"../../Debug/NamedPipe.lib")


void main(char** argv, int argc)
{
	char message[100] = "my message";
	char buffer[100];
	CNamedPipeClient client;

	

	printf("CLIENT: attempting connection to server\n");
	bool bConnected= client.connectToServer("myPipe");
	while (!bConnected)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		bConnected = client.connectToServer("myPipe");
	}

	printf("CLIENT: Client connected\n");
	int numBytes = client.writeBuffer(message, strlen(message)+1);
	printf("CLIENT: %d bytes written: %s\n", numBytes, message);

	printf("CLIENT: closing connection");
	client.closeConnection();
}