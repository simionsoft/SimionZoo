#pragma once

#define MAX_PIPE_NAME_SIZE 1024

class CNamedPipe
{
protected:
	void* m_pipeHandle;
	char m_pipeFullName[MAX_PIPE_NAME_SIZE];
	//wchar_t m_pipeFullName[MAX_PIPE_NAME_SIZE];
	//if bAddPrefix, this method appends the pipeName (i.e. "myPipe") to the standard pipe name prefix
	//leaves the result in m_pipeFullName
	void setPipeName(const char* pipeName,bool bAddPrefix= true,int id= -1);

public:
	CNamedPipe();
	virtual ~CNamedPipe();

	int writeBuffer(const void* pBuffer, int numBytes);
	int readToBuffer(void *pBuffer, int numBytes);
	char* getPipeFullName() { return m_pipeFullName; }
	bool isConnected();
};

class CNamedPipeServer: public CNamedPipe
{
	
public:
	CNamedPipeServer();
	virtual ~CNamedPipeServer();

	bool openNamedPipeServer(const char*);
	bool openUniqueNamedPipeServer(char*);
	bool waitForClientConnection();
	void closeServer();
};

class CNamedPipeClient: public CNamedPipe
{
public:
	CNamedPipeClient();
	virtual ~CNamedPipeClient();

	bool connectToServer(const char*,bool addPrefix= true);
	void closeConnection();
};