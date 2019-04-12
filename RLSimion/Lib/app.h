#pragma once

#include <vector>
#include <unordered_map>
using namespace std;

#include "parameters.h"
#include "mem-manager.h"
#include "run-time-requirements.h"
#include "../Common/named-var-set.h"
#include "../Common/wire-handler.h"

#include "../../tools/System/Timer.h"
#define MAX_PATH_SIZE 1024

class Renderer;
class IInputHandler;
class Text2D;
class Meter2D;
class FunctionSampler;
class FunctionViewer;
class ConfigNode;
class ConfigFile;
class Logger;
class World;
class Experiment;
class SimGod;
class StateActionFunction;
class Wire;

enum Device{ CPU, GPU };



class SimionApp: public WireHandler
{

private:
	static SimionApp* m_pAppInstance;

	ConfigFile* m_pConfigDoc;
	string m_directory;
	string m_configFile;

	//Run-time requirements/support
	unsigned int m_numCPUCores = 1;

	RunTimeRequirements m_commonRequirements;
	unordered_map<string, RunTimeRequirements*> m_targetPlatformRequirements;

	unordered_map<string, StateActionFunction*> m_pStateActionFunctions = {};
	unordered_map<string, Wire*> m_wires = {};

	//is this app being run remotely?
	//by default, we assume it is in Release mode
	//can be overriden using setExecutedRemotely()
#ifdef _DEBUG
	bool m_bRemoteExecution = false;
#else
	bool m_bRemoteExecution = true;
#endif

	void addPlatform(string platform);

public:
	SimionApp(ConfigNode* pParameters);
	virtual ~SimionApp();

	void run();

	static SimionApp* get();

	MemManager<SimionMemPool>* pMemManager;
	CHILD_OBJECT<Logger> pLogger;
	CHILD_OBJECT<World> pWorld;
	CHILD_OBJECT<Experiment> pExperiment;
	CHILD_OBJECT<SimGod> pSimGod;

	//Drawable functions can be added in initialization and drawn if "-local" argument is set
	void registerStateActionFunction(string name, StateActionFunction* pFunction);
	const unordered_map<string, StateActionFunction*> getStateActionFunctions() const { return m_pStateActionFunctions; }

	//Wires: connections between inputs/outputs
	void wireRegister(string name);
	void wireRegister(string name, double minimum, double maximum);
	Wire* wireGet(string name);

	//Run-time requirements
	//Subclasses should call these methods to let know what input/output files will be required at run-time
	//in order to be executed remotely
	//The first two methods should be used to register platform-specific files, and the latter two should be
	//used to register files common to all platforms
	void registerTargetPlatformInputFile(const char* targetPlatform, const char* filepath, const char* rename = 0);
	void registerTargetPlatformOutputFile(const char* targetPlatform, const char* filepath);
	void registerInputFile(const char* filepath, const char* rename= 0);
	void registerOutputFile(const char* filepath);

	void printRequirements();

	void setConfigFile(string);
	string getOutputDirectory();
	string getConfigFile();

	static const char* getArgValue(int argc, char** argv, const char* argName);
	static bool flagPassed(int argc, char** argv, const char* flagName);

	void setExecutedRemotely(bool remote);
	bool isExecutedRemotely();

	void setNumCPUCores(unsigned int numCPUCores) { m_numCPUCores = numCPUCores; }
	unsigned int getNumCPUCores() { return m_numCPUCores; }
	
	void setPreferredDevice(Device device);

private:
	//Rendering
	Timer m_timer;
	Renderer *m_pRenderer = 0;
	IInputHandler *m_pInputHandler = 0;

	Text2D* m_pProgressText = 0;
	vector<Meter2D*> m_pStatsUIMeters;
	vector<Meter2D*> m_pStateUIMeters;
	vector<Meter2D*> m_pActionUIMeters;

	const int m_numSamplesPerDim = 16;
	const unsigned int m_functionViewUpdateStepFreq = 100;
	unordered_map<FunctionViewer*, FunctionSampler*> m_pFunctionViews;
	vector<FunctionSampler*> m_pFunctionSamplers;
	void initFunctionSamplers(State* s, Action* a);

	void update2DMeters(State* s, Action* a);

	void initRenderer(string sceneFile, State* s, Action* a);

	void updateScene(State* s, Action* a);

	bool m_bUseOfflineTraining;
	FILE_PATH_PARAM m_offlineTrainingSampleFile;

public:
	vector<FunctionSampler*> getFunctionSamplers() { return m_pFunctionSamplers; }
};
