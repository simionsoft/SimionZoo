#include "NamedPipe.h"
#include <Windows.h>
#include <string>
#include <thread>

#define NUM_MAX_CONNECTION_ATTEMPTS 10

//NamedPipe: Common stuff
NamedPipe::NamedPipe()
{
	m_pipeHandle = (unsigned long long int)INVALID_HANDLE_VALUE;
}

NamedPipe::~NamedPipe()
{
	if (m_pipeHandle != (unsigned long long int)INVALID_HANDLE_VALUE)
	{
		if (m_bVerbose) printf("Handle closed %llu\n", m_pipeHandle);
		CloseHandle((void*)m_pipeHandle);
	}
}

bool NamedPipe::isConnected()
{
	return (m_pipeHandle != (unsigned long long int)INVALID_HANDLE_VALUE);
}

void NamedPipe::setPipeName(const char* pipeName,bool bAddPrefix,int id)
{
	//this method builds the name of the pipe in m_pipeFullName
	//if bAddPrefix, it prepends the pipeName (i.e. "myPipe") with the standard pipe prefix in windows
	//if id>0, then the base name is appended with the id number until a new pipe can be opened with that name
	if (bAddPrefix)
	{
		if (id < 0)
			sprintf_s(m_pipeFullName, "\\\\.\\pipe\\%s", pipeName);
		else
			sprintf_s(m_pipeFullName, "\\\\.\\pipe\\%s-%d", pipeName, id);
	}
	else
	{
		if (id < 0)
			sprintf_s(m_pipeFullName, "%s", pipeName);
		else
			sprintf_s(m_pipeFullName, "%s-%d", pipeName, id);
	}
}

int NamedPipe::writeBuffer(const void* pBuffer, int numBytes)
{
	DWORD bytesWritten= 0;
	if (m_pipeHandle != (unsigned long long int) INVALID_HANDLE_VALUE)
	{
		WriteFile((void*)m_pipeHandle, pBuffer, numBytes, &bytesWritten, NULL);
		if (m_bVerbose) printf("Written in pipe %s %d bytes\n", m_pipeFullName, numBytes);
	}
	else if (m_bVerbose) printf("Error: couldn't write in pipe\n");

	return bytesWritten;
}

int NamedPipe::readToBuffer(void *pBuffer, int numBytes)
{
	DWORD bytesRead= 0;
	if (m_pipeHandle != (unsigned long long int) INVALID_HANDLE_VALUE)
	{
		ReadFile((void*)m_pipeHandle, pBuffer, numBytes, &bytesRead, NULL);
		if (m_bVerbose) printf("Read from pipe %s %d bytes\n", m_pipeFullName, bytesRead);
	}
	return bytesRead;
}


//CNamedPipeServer
NamedPipeServer::NamedPipeServer()
{
	m_pipeHandle = (unsigned long long int) INVALID_HANDLE_VALUE;
}

NamedPipeServer::~NamedPipeServer()
{
	closeServer();
}

bool NamedPipeServer::openNamedPipeServer()
{
	m_pipeHandle = (unsigned long long int) CreateNamedPipe(m_pipeFullName,
		PIPE_ACCESS_DUPLEX | PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | FILE_FLAG_FIRST_PIPE_INSTANCE,
		PIPE_WAIT,
		1,
		1024 * 16,
		1024 * 16,
		NMPWAIT_USE_DEFAULT_WAIT,
		NULL);

	if (m_pipeHandle == (unsigned long long int) INVALID_HANDLE_VALUE)
	{
		if (m_bVerbose) printf("Created pipe server %s\n", m_pipeFullName);
		return false;
	}
	if (m_bVerbose) printf("Error: couldn't create pipe server %s\n", m_pipeFullName);
	return true;
}

#define NUM_MAX_PIPE_SERVERS_PER_MACHINE 100
bool NamedPipeServer::openUniqueNamedPipeServer(char* pipeName)
{
	int id = 0;
	do
	{
		setPipeName(pipeName,true,id);

		m_pipeHandle = (unsigned long long int) CreateNamedPipe(m_pipeFullName,
			PIPE_ACCESS_DUPLEX | PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | FILE_FLAG_FIRST_PIPE_INSTANCE,
			PIPE_WAIT,
			1,
			1024 * 16,
			1024 * 16,
			NMPWAIT_USE_DEFAULT_WAIT,
			NULL);
		++id;
	} while (m_pipeHandle == (unsigned long long int) INVALID_HANDLE_VALUE && id < NUM_MAX_PIPE_SERVERS_PER_MACHINE);


	if (m_pipeHandle != (unsigned long long int) INVALID_HANDLE_VALUE)
	{
		if (m_bVerbose) printf("Created pipe server %s\n", m_pipeFullName);
		return true;
	}
	if (m_bVerbose) printf("Error: couldn't create pipe server %s\n", m_pipeFullName);
	return false;
}

bool NamedPipeServer::waitForClientConnection()
{
	if (m_bVerbose) printf("Waiting for client connection on pipe %s\n", m_pipeFullName);

	if (ConnectNamedPipe((void*)m_pipeHandle, NULL) != FALSE)   // wait for someone to connect to the pipe
	{
		if (m_bVerbose) printf("Client connected to pipe server %s\n", m_pipeFullName);
		return true;
	}
	return false;
}


void NamedPipeServer::closeServer()
{
	if (m_pipeHandle != (unsigned long long int)INVALID_HANDLE_VALUE)
	{
		if (m_bVerbose) printf("Pipe server closed: %s\n", m_pipeFullName);
		DisconnectNamedPipe((void*) m_pipeHandle);
		CloseHandle((void*) m_pipeHandle);
		m_pipeHandle = (unsigned long long int) INVALID_HANDLE_VALUE;
	}
}

//CNamedPipeClient
NamedPipeClient::NamedPipeClient()
{
	m_pipeHandle = (unsigned long long int) INVALID_HANDLE_VALUE;
}

NamedPipeClient::~NamedPipeClient()
{
	closeConnection();
}
bool NamedPipeClient::connectToServer(const char* pipeName, bool addPipePrefix)
{
	int numAttempts = 0;

	if (m_pipeHandle != (unsigned long long int) INVALID_HANDLE_VALUE)
		return false;

	setPipeName(pipeName, addPipePrefix);
	do
	{
		m_pipeHandle = (unsigned long long int) CreateFile(
			m_pipeFullName,   // pipe name 
			PIPE_ACCESS_DUPLEX | PIPE_TYPE_BYTE | PIPE_READMODE_BYTE,
			PIPE_WAIT,              // no sharing 
			NULL,           // default security attributes
			OPEN_EXISTING,  // opens existing pipe 
			0,              // default attributes 
			NULL);
		numAttempts++;
		
		if (m_pipeHandle == (unsigned long long int) INVALID_HANDLE_VALUE)
			std::this_thread::sleep_for(std::chrono::milliseconds(500));

	} while (m_pipeHandle == (unsigned long long int) INVALID_HANDLE_VALUE && numAttempts<NUM_MAX_CONNECTION_ATTEMPTS);

	if (m_pipeHandle == (unsigned long long int) INVALID_HANDLE_VALUE)
	{
		if (m_bVerbose) printf("Error: couldn't connect to pipe server %s\n", m_pipeFullName);
		return false;
	}

	if (m_bVerbose) printf("Client connected to pipe server %s\n", m_pipeFullName);
	return true;
}

void NamedPipeClient::closeConnection()
{
	if (m_pipeHandle != (unsigned long long int)INVALID_HANDLE_VALUE)
	{
		if (m_bVerbose) printf("Closing connection to pipe server %s\n", m_pipeFullName);
		FlushFileBuffers((void*) m_pipeHandle);
		CloseHandle((void*) m_pipeHandle);
		m_pipeHandle = (unsigned long long int) INVALID_HANDLE_VALUE;
	}
}