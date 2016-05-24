#pragma once

#include "app.h"

class CController;
class CLinearStateVFA;
class CDirFileOutput;
class CParameters;

class CompareControllerVFAApp : public CApp
{
	CController* m_pController;
	int m_numVFAs;
	CLinearStateVFA** m_pVFAs;
	CDirFileOutput *m_pOutputDirFile;
	int m_numSamples;

	char m_outputFilePath[1024];
public:

	CompareControllerVFAApp(CParameters* pParameters);
	virtual ~CompareControllerVFAApp();

	void getInputFiles(CFilePathList& filePathList);
	void getOutputFiles(CFilePathList& filePathList);

	void run();
};