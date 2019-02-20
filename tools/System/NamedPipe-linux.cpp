#include "NamedPipe.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <thread>

#include "CrossPlatform.h"
#include "FileUtils.h"


#define NUM_MAX_CONNECTION_ATTEMPTS 10

//NamedPipe: Common stuff
NamedPipe::NamedPipe()
{
	m_pipeHandle = 0;
}

NamedPipe::~NamedPipe()
{
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
			sprintf(m_pipeFullName, "/tmp/%s", pipeName);
		else
			sprintf(m_pipeFullName, "/tmp/%s-%d", pipeName, id);
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
		logMessage("Error: couldn't write on pipe");
		return 0;
	}
	int numBytesWritten= write((int)m_pipeHandle, pBuffer, numBytes);
	char msg[1024];

	if (m_bVerbose)
	{
		if (numBytesWritten < 0)
			perror(msg);
		logMessage(msg);
		CrossPlatform::Sprintf_s(msg, 1024, "Written %d bytes on pipe %s", numBytesWritten, m_pipeFullName);
		logMessage(msg);
	}
	return numBytesWritten;
}

int NamedPipe::readToBuffer(void *pBuffer, int numBytes)
{
	if (!isConnected())
	{
		logMessage("Error: couldn't read from pipe because it's closed");
		return 0;
	}

	int numBytesRead= read((int)m_pipeHandle, pBuffer, numBytes);
	logMessage("Succesful read from pipe");
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

	int pipeDescriptor = open(m_pipeFullName, O_RDONLY);
	
	if (pipeDescriptor > 0)
	{
		logMessage("Pipe server created");
		m_pipeHandle = pipeDescriptor;
		return true;
	}
	else
	{
		logMessage("Error: couldn't create pipe server");
		return false;
	}
}



bool NamedPipeServer::waitForClientConnection()
{
	//no waiting in the linux version. Read operations will block until the requested number of bytes is read
	char buffer[1024];
	int res= read((int)m_pipeHandle, buffer, 0);
	if (res == -1)
		return false;
	return true;
}


void NamedPipeServer::closeServer()
{
	logMessage("Destroying pipe server");
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
	int pipe = 0;

	setPipeName(pipeName, addPipePrefix);
	do
	{
		pipe= open(m_pipeFullName, O_WRONLY);

		numAttempts++;
		
		if (pipe <= 0)
		{
			logMessage("Failed to connect to pipe");
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		}
		else
			m_pipeHandle = (unsigned long long) pipe;

	} while (m_pipeHandle <= 0 && numAttempts<NUM_MAX_CONNECTION_ATTEMPTS);


	if (isConnected()) logMessage("Client connected to pipe server");
	else logMessage("Error: Client couldn't connect to pipe server");

	return m_pipeHandle > 0;
}

void NamedPipeClient::closeConnection()
{
	if (isConnected())
	{
		logMessage("Client closing connection to pipe server");
		close((int)m_pipeHandle);
		m_pipeHandle = 0;
	}
}