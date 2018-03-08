#pragma once

class CConfigNode;
class CConfigFile;

#include <vector>
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

	//Input/Output files
	std::vector<const char*> m_inputFiles;
	std::vector<const char*> m_inputFilesRenamed; // names to be given in the remote machines to input files
	std::vector<const char*> m_outputFiles;
public:

	CSimionApp();
	virtual ~CSimionApp();

	static CSimionApp* get();

	CMemManager<CSimionMemPool>* pMemManager;
	CHILD_OBJECT<CLogger> pLogger;
	CHILD_OBJECT<CWorld> pWorld;
	CHILD_OBJECT<CExperiment> pExperiment;
	CHILD_OBJECT<CSimGod> pSimGod;


	//Input/Output file registering member functions
	//Subclasses should call these methods to let know what input/output files will be required at run-time
	//in order to be remotely executed
	void registerInputFile(const char* filepath, const char* rename= 0);
	void registerOutputFile(const char* filepath);
	unsigned int getNumInputFiles();
	const char* getInputFile(unsigned int i);
	const char* getInputFileRename(unsigned int i);
	unsigned int getNumOutputFiles();
	const char* getOutputFile(unsigned int i);

	void setConfigFile(string);
	string getOutputDirectory();
	string getConfigFile();
	void printRequirements();

	static const char* getArgValue(int argc, char** argv, char* argName);
	static bool flagPassed(int argc, char** argv, char* flagName);

	//is this app being run remotely?
	//by default, we assume it is in Release mode
	//can be overriden using setExecutedRemotely()
#ifdef _DEBUG
	bool m_bRemoteExecution = false;
#else
	bool m_bRemoteExecution = true;
#endif
	void setExecutedRemotely(bool remote);
	bool isExecutedRemotely();

	virtual void run()= 0;
};
