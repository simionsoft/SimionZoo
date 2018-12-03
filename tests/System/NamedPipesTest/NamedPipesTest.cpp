#include <iostream>
#include <thread>
#include <string.h>
#include <chrono>
#include "../../../tools/System/NamedPipe.h"
#include "../../../tools/System/CrossPlatform.h"
using namespace std;

#define PIPENAME_MAX_SIZE 1024
char pipeName[PIPENAME_MAX_SIZE];

void server()
{
	NamedPipeServer pipeServer;
	pipeServer.setVerbose(true);
	pipeServer.openNamedPipeServer(pipeName);
	cout << "Pipe name= " << pipeName << "\n";

	CrossPlatform::Strcpy_s(pipeName, PIPENAME_MAX_SIZE, pipeServer.getPipeFullName());

	pipeServer.waitForClientConnection();

	char buffer[1024];
	int numBytesRead = 1;
	while (numBytesRead != 0)
	{
		numBytesRead = pipeServer.readToBuffer(buffer, 1024);
		if (numBytesRead > 0)
			cout << "Server read from pipe: " << buffer << "\n";
		else
			cout << "Server: read operation returned 0\n";
	}
	cout << "Server ended\n";
}

void client()
{
	NamedPipeClient pipeClient;
	pipeClient.setVerbose(true);
	cout << "Connecting to server: " << pipeName << "\n";
	pipeClient.connectToServer(pipeName, true);

	const char* message = "Hello my master!";
	pipeClient.writeBuffer(message, strlen(message) + 1);
	pipeClient.closeConnection();
	cout << "Client ended\n";
}

int main(int argc, char** argv)
{
	if (argc <= 1)
		CrossPlatform::Sprintf_s(pipeName, PIPENAME_MAX_SIZE, "test-pipe");
	else
		CrossPlatform::Strcpy_s(pipeName, PIPENAME_MAX_SIZE, argv[1]);

	cout << "Creating server process\n";
	thread serverThread(server);
	serverThread.detach();

	this_thread::sleep_for(chrono::duration<double>(2));
	cout << "Creating client process\n";
	thread clientThread(client);

	cout << "Waiting for client process to end\n";
	clientThread.join();
	this_thread::sleep_for(chrono::duration<double>(15));
	cout << "Test ended";
}

