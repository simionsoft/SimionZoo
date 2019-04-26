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
#include <string.h>
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

#define NUM_MAX_ARGUMENTS 5
#define MAX_COMMAND_LINE_LENGTH 1024
bool Process::spawn(const char* commandLine)
{
	int returnCode;

	if (m_bVerbose) cout << "Forking process\n";

	returnCode = fork();

	if (returnCode != 0)
	{
		m_handle = (unsigned long long int) returnCode;

		if (m_bVerbose) cout << "Parent process creating process: " << commandLine << "\n";
	}
	else
	{
		//separate arguments
		int numArguments = 0;
		char* arguments[NUM_MAX_ARGUMENTS];
		char commandLineCopy[MAX_COMMAND_LINE_LENGTH];
		strncpy(commandLineCopy, commandLine, MAX_COMMAND_LINE_LENGTH);

		for (size_t i = 1; i < strlen(commandLineCopy) && numArguments<NUM_MAX_ARGUMENTS-1; i++)
		{
			if (commandLineCopy[i] == ' ')
			{
				commandLineCopy[i] = 0;
				if (i< strlen(commandLineCopy)-2)
					arguments[numArguments] = &commandLineCopy[i + 1];
			}
		}
		switch (numArguments)
		{
		case 0: returnCode = execl(commandLineCopy, commandLine, NULL); break;
		case 2: returnCode = execl(commandLineCopy, commandLine, arguments[0], arguments[1], NULL); break;
		case 3: returnCode = execl(commandLineCopy, commandLine, arguments[0], arguments[1], arguments[2], NULL); break;
		case 4: returnCode = execl(commandLineCopy, commandLine, arguments[0], arguments[1], arguments[2], NULL); break;
		case 5: default: returnCode = execl(commandLineCopy, commandLine, arguments[0], arguments[1], arguments[2], NULL); break;
		}

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