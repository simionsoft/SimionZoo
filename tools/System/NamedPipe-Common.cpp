#include "NamedPipe.h"

#include <iostream>

void NamedPipe::logMessage(const char* message)
{
	if (m_bVerbose)
	{
		std::cout << message << "\n";
	}
}

bool NamedPipeServer::openNamedPipeServer(const char* pipeName)
{
	setPipeName(pipeName, true);

	return openNamedPipeServer();
}

#define NUM_MAX_PIPE_SERVERS_PER_MACHINE 100
bool NamedPipeServer::openUniqueNamedPipeServer(const char* pipeName)
{
	bool serverCreated = false;
	int id = 0;
	do
	{
		setPipeName(pipeName, true, id);

		serverCreated = openNamedPipeServer();

		++id;

	} while (!serverCreated && id < NUM_MAX_PIPE_SERVERS_PER_MACHINE);

	return serverCreated;
}