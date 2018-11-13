#include "Process.h"
#include <Windows.h>
#include <memory>

Process::Process()
{
	m_handle = INVALID_HANDLE_VALUE;
}

Process::~Process()
{
	//If the process is still running when the owner "dies", kill it
	kill();
}

void Process::kill()
{
	if (m_handle != INVALID_HANDLE_VALUE)
	{
		TerminateProcess(m_handle, 0);
		CloseHandle(m_handle);
		m_handle = INVALID_HANDLE_VALUE;
	}
}

bool Process::spawn(const char* commandLine, bool bAwait)
{
	STARTUPINFO startupInfo;
	PROCESS_INFORMATION processInformation;

	memset(&startupInfo, 0, sizeof(STARTUPINFO));
	startupInfo.wShowWindow = true;

	bool bSuccess = ( TRUE== CreateProcess(NULL,(char*) commandLine, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL
		, &startupInfo, &processInformation));
	if (bSuccess)
	{
		m_handle = processInformation.hProcess;

		if (bAwait)
			WaitForSingleObject(m_handle, INFINITE); // so far, no sense using a timeout
	}

	return bSuccess;
}

bool Process::isRunning()
{
	DWORD returnCode;

	if (m_handle != INVALID_HANDLE_VALUE)
	{
		GetExitCodeProcess(m_handle, &returnCode);

		return returnCode == STILL_ACTIVE;
	}
	return false;
}