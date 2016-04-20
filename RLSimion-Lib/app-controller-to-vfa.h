#pragma once
#include "app.h"

class CLinearStateVFA;
class CController;
class CDirFileOutput;

class ControllerToVFAApp : public CApp
{
	CDirFileOutput *m_pOutputDirFile;
	CController* m_pController;
	int m_numVFAs;
	CLinearStateVFA** m_pVFAs;
public:

	ControllerToVFAApp(int argc,char* argv[]);
	~ControllerToVFAApp();

	void getInputFiles(CFilePathList& filePathList);
	void getOutputFiles(CFilePathList& filePathList);

	void run();
};