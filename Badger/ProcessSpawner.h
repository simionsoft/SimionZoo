#pragma once


class CProcessSlot
{
	void *pStartupInfo;
	void *pProcessInfo;
public:
	CProcessSlot();
	~CProcessSlot();
	void reset();
	void run(char* commandLine);
	void* getProcessHandle();
	void* getThreadHandle();
};


class CProcessSpawner
{
	CProcessSlot** m_pProcessSlots;
	void** m_pHandles;

	int m_numSlotsUsed;
	int m_numSlots;

	void getHandles();
public:
	CProcessSpawner(int numMaxProcesses);
	~CProcessSpawner();

	void spawnOrWait(char* commandLine);
	void waitAll();
};