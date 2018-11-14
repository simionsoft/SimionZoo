#include <iostream>
#include <thread>
#include <string.h>
#include <chrono>
#include "../../../tools/System/NamedPipe.h"
using namespace std;

#define PIPENAME_MAX_SIZE 1024
char pipeName[PIPENAME_MAX_SIZE];

void server()
{
	NamedPipeServer pipeServer;
	pipeServer.setVerbose(true);
	pipeServer.openUniqueNamedPipeServer(pipeName);
#ifdef _WIN32
	strcpy_s(pipeName, PIPENAME_MAX_SIZE, pipeServer.getPipeFullName());
#else
	strcpy(pipeName, pipeServer.getPipeFullName());
#endif
	pipeServer.waitForClientConnection();

	char buffer[1024];
	int numBytesRead= pipeServer.readToBuffer(buffer, 1024);
	cout << "Server read from pipe: " << buffer << "\n";
	cout << "Server ended\n";
}

void client()
{
	NamedPipeClient pipeClient;
	pipeClient.setVerbose(true);
	pipeClient.connectToServer(pipeName, false);

	const char* message = "Hello my master!";
	pipeClient.writeBuffer(message, strlen(message) + 1);
	cout << "Client ended\n";
}

int main()
{
#ifdef _WIN32
	sprintf_s(pipeName, "test-pipe");
#else
	sprintf(pipeName, "test-pipe");
#endif
	cout << "Creating server process\n";
	thread serverThread(server);
	serverThread.detach();

	this_thread::sleep_for(chrono::duration<double>(2));
	cout << "Creating client process\n";
	thread clientThread(client);

	cout << "Waiting for client process to end\n";
	clientThread.join();
	cout << "Test ended";
}

