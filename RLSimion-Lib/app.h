#pragma once

class CConfigNode;
class CConfigFile;

class CFilePathList;

#include "worlds/world.h"
#include "logger.h"
#include "experiment.h"
#include "SimGod.h"
#include "parameters.h"
#include "mem-manager.h"

class CSimionApp
{
private:
	static CSimionApp* m_pAppInstance;
protected:
	CConfigFile* m_pConfigDoc;
	string m_directory;
	string m_configFile;
public:

	CSimionApp();
	virtual ~CSimionApp();

	static CSimionApp* get();

	CMemManager<CSimionMemPool>* pMemManager;
	CHILD_OBJECT<CLogger> pLogger;
	CHILD_OBJECT<CWorld> pWorld;
	CHILD_OBJECT<CExperiment> pExperiment;
	CHILD_OBJECT<CSimGod> pSimGod;

	virtual void getInputFiles(CFilePathList& filePathList) = 0;
	virtual void getOutputFiles(CFilePathList& filePathList) = 0;
	void setConfigFile(string);
	string getOutputDirectory();
	string getConfigFile();
	void printInputOutputFiles();

	static const char* getArgValue(int argc, char** argv, char* argName);
	static bool flagPassed(int argc, char** argv, char* flagName);

	//is this app being run remotely?
	bool isExecutedRemotely();

	virtual void run()= 0;
};
