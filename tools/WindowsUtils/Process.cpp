#include "Process.h"
#include <Windows.h>
#include <memory>

CProcess::CProcess()
{
	m_handle = INVALID_HANDLE_VALUE;
}

CProcess::~CProcess()
{
	if (m_handle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_handle);
	}
}

bool CProcess::spawn(const char* exeFile, char* args)
{
	STARTUPINFO startupInfo;
	PROCESS_INFORMATION processInformation;

	memset(&startupInfo, 0, sizeof(STARTUPINFO));
	startupInfo.wShowWindow = true;

	bool bSuccess = (bool) CreateProcess(exeFile, args, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL
		, &startupInfo, &processInformation);
	if (bSuccess)
		m_handle = processInformation.hProcess;

	return bSuccess;
}
