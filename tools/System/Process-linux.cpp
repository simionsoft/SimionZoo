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

#include <signal.h>
#include <unistd.h>
#include <wait.h>
#include <iostream>
using namespace std;

Process::Process()
{
	m_handle = 0;
}

Process::~Process()
{
	//If the process is still running when the owner "dies", stop it
	stop();
}

void Process::stop()
{
	if (m_handle > 0)
	{
		if (m_bVerbose) cout << "Stopping process\n";
		kill((__pid_t)m_handle, SIGKILL);
	}
}

bool Process::spawn(const char* commandLine, bool bAwait, const char* args)
{
	int status;
	int returnCode;

	if (m_bVerbose) cout << "Forking process\n";

	returnCode = fork();

	if (returnCode != 0)
	{
		m_handle = (unsigned long long int) returnCode;

		if (m_bVerbose) cout << "Parent process creating process: " << commandLine << " " << args << "\n";

		if (bAwait)
		{
			if (m_bVerbose) cout << "Waiting for process to finish: " << commandLine << "\n";
			waitpid((__pid_t)m_handle, &status, 0);
			m_handle = 0;
			if (m_bVerbose) cout << "Process finished\n";
		}
	}
	else
	{
		if (args==nullptr)
			returnCode= execl(commandLine, commandLine, NULL);
		else
			returnCode = execl(commandLine, commandLine, args, NULL);

		if (returnCode < 0 && m_bVerbose) cout << "Failed creating process: " << commandLine << "\n";
		_Exit(0);
	}

	return true;
}

bool Process::isRunning()
{
	int status;
	__pid_t returnCode;
	
	if (m_handle > 0)
	{
		returnCode= waitpid((__pid_t)m_handle, &status, WNOHANG);

		if (returnCode == 0)
		{
			if (m_bVerbose) cout << "Child process status: running\n";
			return true;
		}
		else if (m_handle==returnCode)
		{
			if (m_bVerbose)
			{
				if (WIFEXITED(status))
					cout << "Child process status: stopped normally\n";
				else if (WIFSIGNALED(status))
					cout << "Child process status: forced to stop\n";
			}
			return false;
		}
		else
		{
			if (m_bVerbose) cout << "Child process status: finished";
			return false;
		}
	}
	if (m_bVerbose) cout << "Child process status: finished";
	return false;
}

void Process::wait()
{
	if (m_handle > 0)
	{
		if (isRunning())
		{
			if (m_bVerbose) cout << "Waiting for child process to finish\n";
			waitpid((__pid_t)m_handle, 0, 0);
			if (m_bVerbose) cout << "Child process finished\n";
			m_handle = 0;
		}
		else
		{
			if (m_bVerbose) cout << "Child process had already finished\n";
		}
	}
}