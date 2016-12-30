#pragma once

class CProcess
{
	void* m_handle;
public:
	CProcess();
	~CProcess();

	bool spawn(const char* exeFile, char* args= 0);
};