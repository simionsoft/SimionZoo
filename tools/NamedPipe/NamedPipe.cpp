#include "NamedPipe.h"
#include <Windows.h>
#include <string>

//CNamedPipe: Common stuff
CNamedPipe::CNamedPipe()
{
	m_pipeHandle = 0;
}

CNamedPipe::~CNamedPipe()
{
	if (m_pipeHandle)
		CloseHandle(m_pipeHandle);
}

void CNamedPipe::setPipeName(const char* pipeName)
{
	//this method appends the pipeName (i.e. "myPipe") to the standard pipe name prefix
	//and leaves the result in m_pipeFullName
	//size_t convertedChars;

	//wchar_t w_pipeName[512];
	//lstrcpynW(m_pipeFullName, L"\\\\.\\pipe\\", MAX_PIPE_NAME_SIZE);


	//mbstowcs_s(&convertedChars, w_pipeName, 512, pipeName, 512);
	//wcscat_s(m_pipeFullName, w_pipeName);
	sprintf_s(m_pipeFullName, "\\\\.\\pipe\\%s", pipeName);
}

int CNamedPipe::writeBuffer(const void* pBuffer, int numBytes)
{
	DWORD bytesWritten;
	if (m_pipeHandle)
		WriteFile(m_pipeHandle,	pBuffer, numBytes, &bytesWritten,	NULL);
	return bytesWritten;
}

int CNamedPipe::readToBuffer(void *pBuffer, int numBytes)
{
	DWORD bytesRead;
	if (m_pipeHandle)
		ReadFile(m_pipeHandle, pBuffer, numBytes, &bytesRead, NULL);
	return bytesRead;
}


//CNamedPipeServer
CNamedPipeServer::CNamedPipeServer()
{}

CNamedPipeServer::~CNamedPipeServer()
{
	closeServer();
}

bool CNamedPipeServer::openNamedPipeServer(const char* pipeName)
{
	setPipeName(pipeName);
	m_pipeHandle = CreateNamedPipe(m_pipeFullName,
		PIPE_ACCESS_DUPLEX | PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | FILE_FLAG_FIRST_PIPE_INSTANCE,   // FILE_FLAG_FIRST_PIPE_INSTANCE is not needed but forces CreateNamedPipe(..) to fail if the pipe already exists...
		PIPE_WAIT,
		1,
		1024 * 16,
		1024 * 16,
		NMPWAIT_USE_DEFAULT_WAIT,
		NULL);
	if (m_pipeHandle == INVALID_HANDLE_VALUE) return false;
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
	if (m_pipeHandle)
	{
		DisconnectNamedPipe(m_pipeHandle);
		CloseHandle(m_pipeHandle);
		m_pipeHandle = 0;
	}
}

//CNamedPipeClient
CNamedPipeClient::CNamedPipeClient()
{}
CNamedPipeClient::~CNamedPipeClient()
{
	closeConnection();
}
bool CNamedPipeClient::connectToServer(const char* pipeName)
{
	setPipeName(pipeName);

	m_pipeHandle = CreateFile(
		m_pipeFullName,   // pipe name 
		PIPE_ACCESS_DUPLEX | PIPE_TYPE_BYTE | PIPE_READMODE_BYTE,
		PIPE_WAIT,              // no sharing 
		NULL,           // default security attributes
		OPEN_EXISTING,  // opens existing pipe 
		0,              // default attributes 
		NULL);          // no template file 

	if (m_pipeHandle == INVALID_HANDLE_VALUE) return false;

	return true;
}

void CNamedPipeClient::closeConnection()
{
	if (m_pipeHandle)
	{
		FlushFileBuffers(m_pipeHandle);
		CloseHandle(m_pipeHandle);
		m_pipeHandle = 0;
	}
}