#pragma once

class ConfigNode;
class ConfigFile;
class Logger;
class World;
class Experiment;
class SimGod;

#include <vector>
#include "parameters.h"
#include "mem-manager.h"

class SimionApp
{
private:
	static SimionApp* m_pAppInstance;
protected:
	ConfigFile* m_pConfigDoc;
	string m_directory;
	string m_configFile;

	//Input/Output files
	std::vector<const char*> m_inputFiles;
	std::vector<const char*> m_inputFilesRenamed; // names to be given in the remote machines to input files
	std::vector<const char*> m_outputFiles;
public:

	SimionApp();
	virtual ~SimionApp();

	static SimionApp* get();

	MemManager<SimionMemPool>* pMemManager;
	CHILD_OBJECT<Logger> pLogger;
	CHILD_OBJECT<World> pWorld;
	CHILD_OBJECT<Experiment> pExperiment;
	CHILD_OBJECT<SimGod> pSimGod;


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
