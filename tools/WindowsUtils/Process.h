#pragma once

class CProcess
{
	void* m_handle;
public:
	CProcess();
	~CProcess();

	void kill();
	bool spawn(const char* commandLine, bool bAwait= false);
	bool isRunning();
};