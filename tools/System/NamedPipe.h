#pragma once

#define MAX_PIPE_NAME_SIZE 1024

class NamedPipe
{
protected:

	bool m_bVerbose = false;

	unsigned long long int m_pipeHandle;

	char m_pipeFullName[MAX_PIPE_NAME_SIZE];

	//if bAddPrefix, this method appends the pipeName (i.e. "myPipe") to the standard pipe name prefix
	//leaves the result in m_pipeFullName
	void setPipeName(const char* pipeName,bool bAddPrefix= true,int id= -1);

	void logMessage(const char* message);

public:
	NamedPipe();
	virtual ~NamedPipe();

	int writeBuffer(const void* pBuffer, int numBytes);
	int readToBuffer(void *pBuffer, int numBytes);
	char* getPipeFullName() { return m_pipeFullName; }
	bool isConnected();

	void setVerbose(bool set) { m_bVerbose = set; }
};

class NamedPipeServer: public NamedPipe
{
	bool openNamedPipeServer();
public:
	NamedPipeServer();
	virtual ~NamedPipeServer();

	//This method should be used when the name could have already been used from another instance of the program
	//It appends an identifier to the name given until it finds an unused name. getPipeFullName() should be used after
	//to retrieve the actual name of the pipe so that the client can open it
	bool openUniqueNamedPipeServer(const char*);
	bool waitForClientConnection();
	void closeServer();
};

class NamedPipeClient: public NamedPipe
{
public:
	NamedPipeClient();
	virtual ~NamedPipeClient();

	bool connectToServer(const char*,bool addPrefix= true);
	void closeConnection();
};