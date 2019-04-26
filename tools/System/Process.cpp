/*
	SimionZoo: A framework for online model-free Reinforcement Learning on continuous
	control problems

	Copyright (c) 2016 SimionSoft. https://github.com/simionsoft

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

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

bool Process::spawn(const char* commandLine)
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