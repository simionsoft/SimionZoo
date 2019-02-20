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
		kill((__pid_t)m_handle, SIGTERM);
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

		if (returnCode < 0)
		{
			if (m_bVerbose) cout << "Failed creating process: " << commandLine << "\n";
			return false;
		}
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