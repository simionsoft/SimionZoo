#pragma once

class Process
{
	void* m_handle;
public:
	Process();
	~Process();

	void kill();
	bool spawn(const char* commandLine, bool bAwait= false);
	bool isRunning();
};