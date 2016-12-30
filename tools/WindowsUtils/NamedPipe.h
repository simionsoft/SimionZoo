#pragma once

#define MAX_PIPE_NAME_SIZE 1024

class CNamedPipe
{
protected:
	void* m_pipeHandle;
	char m_pipeFullName[MAX_PIPE_NAME_SIZE];
	//wchar_t m_pipeFullName[MAX_PIPE_NAME_SIZE];
	//this method appends the pipeName (i.e. "myPipe") to the standard pipe name prefix
	//and leaves the result in m_pipeFullName
	void setPipeName(const char* pipeName);

public:
	CNamedPipe();
	virtual ~CNamedPipe();

	int writeBuffer(const void* pBuffer, int numBytes);
	int readToBuffer(void *pBuffer, int numBytes);
};

class CNamedPipeServer: public CNamedPipe
{
	
public:
	CNamedPipeServer();
	virtual ~CNamedPipeServer();

	bool openNamedPipeServer(const char*);
	bool waitForClientConnection();
	void closeServer();
};

class CNamedPipeClient: public CNamedPipe
{
public:
	CNamedPipeClient();
	virtual ~CNamedPipeClient();

	bool connectToServer(const char*);
	void closeConnection();
};