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

	static const char* getInputs(const char* xmlConfigFile);
	static const char* getOutputs(const char* xmlConfigFile);

	void run();
};