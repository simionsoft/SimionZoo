#include "NamedPipe.h"
#include <Windows.h>
#include <string>
#include <thread>

#define NUM_MAX_CONNECTION_ATTEMPTS 10

//CNamedPipe: Common stuff
CNamedPipe::CNamedPipe()
{
	m_pipeHandle = INVALID_HANDLE_VALUE;
}

CNamedPipe::~CNamedPipe()
{
	if (m_pipeHandle!=INVALID_HANDLE_VALUE)
		CloseHandle(m_pipeHandle);
}

bool CNamedPipe::isConnected()
{
	return (m_pipeHandle != INVALID_HANDLE_VALUE);
}

void CNamedPipe::setPipeName(const char* pipeName,bool bAddPrefix,int id)
{
	//this method appends the pipeName (i.e. "myPipe") to the standard pipe name prefix
	//and leaves the result in m_pipeFullName
	//if id>0, then the base name is appended with the id until a server can be opened with that name
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

int CNamedPipe::writeBuffer(const void* pBuffer, int numBytes)
{
	DWORD bytesWritten= 0;
	if (m_pipeHandle!=INVALID_HANDLE_VALUE)
		WriteFile(m_pipeHandle,	pBuffer, numBytes, &bytesWritten,	NULL);
	return bytesWritten;
}

int CNamedPipe::readToBuffer(void *pBuffer, int numBytes)
{
	DWORD bytesRead= 0;
	if (m_pipeHandle!= INVALID_HANDLE_VALUE)
		ReadFile(m_pipeHandle, pBuffer, numBytes, &bytesRead, NULL);
	return bytesRead;
}


//CNamedPipeServer
CNamedPipeServer::CNamedPipeServer()
{
	m_pipeHandle = INVALID_HANDLE_VALUE;
}

CNamedPipeServer::~CNamedPipeServer()
{
	closeServer();
}

bool CNamedPipeServer::openNamedPipeServer(const char* pipeName)
{
	setPipeName(pipeName);

	m_pipeHandle = CreateNamedPipe(m_pipeFullName,
		PIPE_ACCESS_DUPLEX | PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | FILE_FLAG_FIRST_PIPE_INSTANCE,
		PIPE_WAIT,
		1,
		1024 * 16,
		1024 * 16,
		NMPWAIT_USE_DEFAULT_WAIT,
		NULL);

	if (m_pipeHandle == INVALID_HANDLE_VALUE)
		return false;

	return true;
}

#define NUM_MAX_PIPE_SERVERS_PER_MACHINE 100
bool CNamedPipeServer::openUniqueNamedPipeServer(char* pipeName)
{
	int id = 0;
	do
	{
		setPipeName(pipeName,true,id);

		m_pipeHandle = CreateNamedPipe(m_pipeFullName,
			PIPE_ACCESS_DUPLEX | PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | FILE_FLAG_FIRST_PIPE_INSTANCE,
			PIPE_WAIT,
			1,
			1024 * 16,
			1024 * 16,
			NMPWAIT_USE_DEFAULT_WAIT,
			NULL);
		++id;
	} while (m_pipeHandle == INVALID_HANDLE_VALUE && id < NUM_MAX_PIPE_SERVERS_PER_MACHINE);

	if (m_pipeHandle == INVALID_HANDLE_VALUE)
		return false;

	return true;
}

bool CNamedPipeServer::waitForClientConnection()
{
	if (ConnectNamedPipe(m_pipeHandle, NULL) != FALSE)   // wait for someone to connect to the pipe
		return true;
	return false;
}


void CNamedPipeServer::closeServer()
{
	if (m_pipeHandle!= INVALID_HANDLE_VALUE)
	{
		DisconnectNamedPipe(m_pipeHandle);
		CloseHandle(m_pipeHandle);
		m_pipeHandle = INVALID_HANDLE_VALUE;
	}
}

//CNamedPipeClient
CNamedPipeClient::CNamedPipeClient()
{
	m_pipeHandle = INVALID_HANDLE_VALUE;
}

CNamedPipeClient::~CNamedPipeClient()
{
	closeConnection();
}
bool CNamedPipeClient::connectToServer(const char* pipeName, bool addPipePrefix)
{
	int numAttempts = 0;

	if (m_pipeHandle != INVALID_HANDLE_VALUE)
		return false;

	setPipeName(pipeName, addPipePrefix);
	do
	{
		m_pipeHandle = CreateFile(
			m_pipeFullName,   // pipe name 
			PIPE_ACCESS_DUPLEX | PIPE_TYPE_BYTE | PIPE_READMODE_BYTE,
			PIPE_WAIT,              // no sharing 
			NULL,           // default security attributes
			OPEN_EXISTING,  // opens existing pipe 
			0,              // default attributes 
			NULL);
		numAttempts++;
		
		if (m_pipeHandle == INVALID_HANDLE_VALUE)
			std::this_thread::sleep_for(std::chrono::milliseconds(500));

	} while (m_pipeHandle == INVALID_HANDLE_VALUE && numAttempts<NUM_MAX_CONNECTION_ATTEMPTS);

	if (m_pipeHandle == INVALID_HANDLE_VALUE)
		return false;

	return true;
}

void CNamedPipeClient::closeConnection()
{
	if (m_pipeHandle!=INVALID_HANDLE_VALUE)
	{
		FlushFileBuffers(m_pipeHandle);
		CloseHandle(m_pipeHandle);
		m_pipeHandle = INVALID_HANDLE_VALUE;
	}
}