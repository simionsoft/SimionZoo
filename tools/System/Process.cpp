#include "Process.h"
#include <Windows.h>
#include <memory>
#include <iostream>
using namespace std;

Process::Process()
{
	m_handle = (long long) INVALID_HANDLE_VALUE;
}

Process::~Process()
{
	//If the process is still running when the owner "dies", stop it
	stop();
}

void Process::stop()
{
	if (m_handle != (long long)INVALID_HANDLE_VALUE)
	{
		if (m_bVerbose) cout << "Stopping process\n";
		TerminateProcess((void*) m_handle, 0);
		CloseHandle((void*) m_handle);
		m_handle = (long long) INVALID_HANDLE_VALUE;
	}
}

bool Process::spawn(const char* commandLine, bool bAwait, const char* args)
{
	STARTUPINFO startupInfo;
	PROCESS_INFORMATION processInformation;

	memset(&startupInfo, 0, sizeof(STARTUPINFO));
	startupInfo.wShowWindow = true;

	bool bSuccess = ( TRUE== CreateProcess(NULL,(char*) commandLine, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL
		, &startupInfo, &processInformation));
	if (bSuccess)
	{
		if (m_bVerbose) cout << "Creating process: " << commandLine << "\n";
		m_handle = (long long) processInformation.hProcess;

		if (bAwait)	wait();
	}
	else if (m_bVerbose) cout << "Failed creating process: " << commandLine << "\n";

	return bSuccess;
}

bool Process::isRunning()
{
	DWORD returnCode;

	if (m_handle != (long long) INVALID_HANDLE_VALUE)
	{
		GetExitCodeProcess((void*) m_handle, &returnCode);

		if (m_bVerbose)
		{
			if (returnCode == STILL_ACTIVE)
				cout << "Process status request: still active\n";
			else cout << "Process status request: finished\n";
		}

		return returnCode == STILL_ACTIVE;
	}
	return false;
}

void Process::wait()
{
	if (m_bVerbose) cout << "Waiting for process to finish\n";
	WaitForSingleObject((void*)m_handle, INFINITE); // so far, no sense using a timeout
	if (m_bVerbose) cout << "Process finished: ";
}