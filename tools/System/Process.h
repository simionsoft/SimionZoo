#pragma once

class Process
{
	long long int m_handle;
	bool m_bVerbose = false;
public:
	Process();
	~Process();

	void stop();
	bool spawn(const char* commandLine);
	bool isRunning();
	void wait();

	void setVerbose(bool set) { m_bVerbose = set; }
};
