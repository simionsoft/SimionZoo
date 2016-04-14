#pragma once

#include "app.h"

class CController;
class CLinearStateVFA;
class CDirFileOutput;

class CompareControllerVFAApp : public CApp
{
	CController* m_pController;
	int m_numVFAs;
	CLinearStateVFA** m_pVFAs;
	CDirFileOutput *m_pOutputDirFile;
	int m_numSamples;
public:

	CompareControllerVFAApp(int argc, char* argv[]);
	~CompareControllerVFAApp();

	void run();
};