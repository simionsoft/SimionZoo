#include "stdafx.h"
#include "ProcessSpawner.h"
#include <string>

//STARTUPINFO *pStartupInfo;
//PROCESS_INFORMATION *pProcessInfo;


CProcessSlot::CProcessSlot()
{
	pStartupInfo = malloc(sizeof(STARTUPINFO));
	pProcessInfo = malloc(sizeof(PROCESS_INFORMATION));
	memset(pStartupInfo, 0, sizeof(STARTUPINFO));
	memset(pProcessInfo, 0, sizeof(PROCESS_INFORMATION));
}
CProcessSlot::~CProcessSlot()
{
	free(pStartupInfo);
	free(pProcessInfo);
}

void CProcessSlot::reset()
{
	if (pProcessInfo)
	{
		if (CProcessSlot::getProcessHandle())
			CloseHandle(CProcessSlot::getProcessHandle());
		if (CProcessSlot::getThreadHandle())
			CloseHandle(CProcessSlot::getThreadHandle());
	}

	memset(pStartupInfo, 0, sizeof(STARTUPINFO));
	((STARTUPINFO*)pStartupInfo)->cb = sizeof(STARTUPINFO);
	memset(pProcessInfo, 0, sizeof(PROCESS_INFORMATION));
}

void CProcessSlot::run(char* commandLine)
{
	wchar_t command[1024];
	size_t convertedChars;
	mbstowcs_s(&convertedChars, command, strlen(commandLine)+1, commandLine, _TRUNCATE);
	int ret = CreateProcess(NULL, command, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, (LPSTARTUPINFOW)pStartupInfo
		,(LPPROCESS_INFORMATION) pProcessInfo);
}

void* CProcessSlot::getProcessHandle()
{
	if (pProcessInfo)
		return ((PROCESS_INFORMATION*)pProcessInfo)->hProcess;
	return 0;
}


void* CProcessSlot::getThreadHandle()
{
	if (pProcessInfo)
		return ((PROCESS_INFORMATION*)pProcessInfo)->hThread;
	return 0;
}



CProcessSpawner::CProcessSpawner(int numMaxProcesses)
{
	m_numSlotsUsed = 0;
	m_numSlots = numMaxProcesses;

	m_pHandles = new void*[m_numSlots];

	m_pProcessSlots = new CProcessSlot* [m_numSlots];
	for (int i = 0; i<m_numSlots; i++)
	{
		m_pProcessSlots[i] = new CProcessSlot;
		m_pProcessSlots[i]->reset();
	}
}

CProcessSpawner::~CProcessSpawner()
{
	delete [] m_pHandles;


	for (int i = 0; i < m_numSlots; i++)
	{
		m_pProcessSlots[i]->reset();

		delete m_pProcessSlots[i];
	}
	delete [] m_pProcessSlots;
}

void CProcessSpawner::getHandles()
{
	for (int i = 0; i < m_numSlotsUsed; i++)
	{
		if (m_pProcessSlots[i])
			m_pHandles[i] = (void*) m_pProcessSlots[i]->getProcessHandle();
		else m_pHandles[i] = 0;
	}
}

void CProcessSpawner::spawnOrWait(char* commandLine)
{
	int processIndex;

	if (m_numSlotsUsed < m_numSlots)
	{
		//we can run the process without waiting
		m_pProcessSlots[m_numSlotsUsed]->run(commandLine);
		m_numSlotsUsed++;
	}
	else
	{
		//we have to wait until one the processes running finishes
		getHandles();
		processIndex = WaitForMultipleObjects(m_numSlotsUsed, (HANDLE*)m_pHandles, FALSE, INFINITE) - WAIT_OBJECT_0;

		m_pProcessSlots[processIndex]->reset();
		m_pProcessSlots[processIndex]->run(commandLine);
	}
}

void CProcessSpawner::waitAll()
{
	if (m_numSlotsUsed)
	{
		getHandles();

		WaitForMultipleObjects(m_numSlotsUsed, (HANDLE*)m_pHandles, TRUE, INFINITE);
	}
}