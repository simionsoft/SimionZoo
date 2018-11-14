#include "NamedPipe.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <thread>


#define NUM_MAX_CONNECTION_ATTEMPTS 10

//NamedPipe: Common stuff
NamedPipe::NamedPipe()
{
	m_pipeHandle = 0;
}

NamedPipe::~NamedPipe()
{
	close((int) m_pipeHandle);
}

bool NamedPipe::isConnected()
{
	return (m_pipeHandle > 0);
}

void NamedPipe::setPipeName(const char* pipeName,bool bAddPrefix,int id)
{
	//this method builds the name of the pipe in m_pipeFullName
	//if bAddPrefix, it prepends the pipeName (i.e. "myPipe") with tmp/
	//if id>0, then the base name is appended with the id number until a new pipe can be opened with that name
	if (bAddPrefix)
	{
		if (id < 0)
			sprintf(m_pipeFullName, "tmp/%s", pipeName);
		else
			sprintf(m_pipeFullName, "tmp/%s-%d", pipeName, id);
	}
	else
	{
		if (id < 0)
			sprintf(m_pipeFullName, "%s", pipeName);
		else
			sprintf(m_pipeFullName, "%s-%d", pipeName, id);
	}
}

int NamedPipe::writeBuffer(const void* pBuffer, int numBytes)
{
	if (!isConnected())
	{
		if (m_bVerbose) printf("Error: couldn't write on pipe %s\n", m_pipeFullName);
		return 0;
	}
	int numBytesWritten= write((int)m_pipeHandle, pBuffer, numBytes);
	if (m_bVerbose) printf("Written %d bytes on pipe %s\n", numBytesWritten, m_pipeFullName);
	return numBytesWritten;
}

int NamedPipe::readToBuffer(void *pBuffer, int numBytes)
{
	if (!isConnected())
	{
		if (m_bVerbose) printf("Error: couldn't read from pipe %s\n", m_pipeFullName);
		return 0;
	}

	int numBytesRead= read((int)m_pipeHandle, pBuffer, numBytes);
	if (m_bVerbose) printf("Read %d bytes from pipe %s\n", numBytesRead, m_pipeFullName);
	return numBytesRead;
}


//CNamedPipeServer
NamedPipeServer::NamedPipeServer()
{
}

NamedPipeServer::~NamedPipeServer()
{
	closeServer();
}

//before calling this function, setPipeName() must be called

bool NamedPipeServer::openNamedPipeServer()
{
	if (mkfifo(m_pipeFullName, S_IWUSR | S_IRUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH) != 0)
		return false;

	int pipeDescriptor = open(m_pipeFullName, O_RDWR);
	
	if (pipeDescriptor >= 0)
	{
		if (m_bVerbose) printf("Created pipe server %s\n", m_pipeFullName);
		m_pipeHandle = pipeDescriptor;
		return true;
	}
	else
	{
		if (m_bVerbose) printf("Error: couldn't create pipe server %s\n", m_pipeFullName);
		return false;
	}
}

#define NUM_MAX_PIPE_SERVERS_PER_MACHINE 100
bool NamedPipeServer::openUniqueNamedPipeServer(char* pipeName)
{
	bool serverCreated = false;
	int id = 0;
	do
	{
		setPipeName(pipeName,false,id);

		if (access(m_pipeFullName, F_OK) == -1)
		{
			//no pipe has been created with that name
			serverCreated= openNamedPipeServer();
		}
		else
			++id;
	} while (!serverCreated && id < NUM_MAX_PIPE_SERVERS_PER_MACHINE);

	return serverCreated;
}

bool NamedPipeServer::waitForClientConnection()
{
	//no waiting in the linux version. Read operations will block until the requested number of bytes is read
	return true;
}


void NamedPipeServer::closeServer()
{
	if (m_bVerbose) printf("Destroying pipe server %s\n", m_pipeFullName);
	unlink(m_pipeFullName);
}

//CNamedPipeClient
NamedPipeClient::NamedPipeClient()
{
}

NamedPipeClient::~NamedPipeClient()
{
	closeConnection();
}

bool NamedPipeClient::connectToServer(const char* pipeName, bool addPipePrefix)
{
	int numAttempts = 0;

	setPipeName(pipeName, addPipePrefix);
	do
	{
		m_pipeHandle= open(m_pipeFullName, O_RDWR);

		numAttempts++;
		
		if (m_pipeHandle <= 0)
			std::this_thread::sleep_for(std::chrono::milliseconds(500));

	} while (m_pipeHandle < 0 && numAttempts<NUM_MAX_CONNECTION_ATTEMPTS);

	if (m_bVerbose)
	{
		if (isConnected()) printf("Client connected to pipe server %s\n", m_pipeFullName);
		else printf("Error: Client couldn't connect to pipe server %s\n", m_pipeFullName);
	}

	return m_pipeHandle > 0;
}

void NamedPipeClient::closeConnection()
{
	printf("Client closing connection to pipe server %s\n", m_pipeFullName);
	close((int) m_pipeHandle);
}