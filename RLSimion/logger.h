#pragma once

#include <vector>
#include "parameters.h"
#include "../tools/WindowsUtils/NamedPipe.h"
#include "stats.h"

class NamedVarSet;
typedef NamedVarSet State;
typedef NamedVarSet Action;
typedef NamedVarSet Reward;
class ConfigNode;
class Descriptor;
class Timer;
class FunctionSampler;

enum MessageType {Progress,Evaluation,Info,Warning, Error};
enum MessageOutputMode {Console,NamedPipe};

class Logger
{
	static const int MAX_FILENAME_LENGTH = 1024;
	static const int BUFFER_SIZE = 10000;

	//functions file: drawable downsampled 2d or 1d versions of the functions learned by the agents
	string m_outputFunctionLogBinary;
	static FILE *m_functionLogFile;

	BOOL_PARAM m_bLogFunctions;
	INT_PARAM m_numFunctionsLogged;

	void openFunctionLogFile(const char* filename);
	void closeFunctionLogFile();
	void logFunctionSample(FunctionSampler* pFunctionSampler);


	//log file
	string m_outputLogDescriptor;
	string m_outputLogBinary;
	static FILE *m_logFile;

	BOOL_PARAM m_bLogEvaluationEpisodes;
	BOOL_PARAM m_bLogTrainingEpisodes;
	DOUBLE_PARAM m_logFreq; //in seconds: time between file logs

	Timer *m_pEpisodeTimer;
	Timer *m_pExperimentTimer;

	double m_episodeRewardSum;
	double m_lastLogSimulationT;

	void openLogFile(const char* fullLogFilename);
	void closeLogFile();

	static void writeLogBuffer(const char* pBuffer, int numBytes);
	void writeLogFileXMLDescriptor(const char* filename);

	void writeNamedVarSetDescriptorToBuffer(char* buffer, const char* id, const Descriptor* pNamedVarSet);
	void writeStatDescriptorToBuffer(char* buffer);
	void writeEpisodeTypesToBuffer(char* buffer);

	void writeExperimentHeader();
	void writeEpisodeHeader();
	void writeEpisodeEndHeader();
	int writeStepHeaderToBuffer(char* buffer, int offset);
	void writeStepData(State* s, Action* a, State* s_p, Reward* r);
	int writeNamedVarSetToBuffer(char* buffer, int offset, const NamedVarSet* pNamedVarSet);
	int writeStatsToBuffer(char* buffer, int offset);

	//stats
	std::vector<IStats *> m_stats;
public:
	static const unsigned int BIN_FILE_VERSION = 2;

	Logger(ConfigNode* pParameters);
	Logger() = default;
	virtual ~Logger();

	//returns whether a specific type of episode is going to be logged
	bool isEpisodeTypeLogged(bool evaluation);

	//METHODS CALLED FROM ANY CLASS
	template <typename T>
	void addVarToStats(string key, string subkey, T& variable)
	{
		m_stats.push_back(new Stats<T>(key, subkey, variable));
	}
	void addVarSetToStats(const char* key, NamedVarSet* varset);

	size_t getNumStats();
	IStats* getStats(unsigned int i);

	void setOutputFilenames();

	static MessageOutputMode m_messageOutputMode;
	static CNamedPipeClient m_outputPipe;
	static bool m_bLogMessagesEnabled;

	//Function called to report progress and error messages
	//static so that it can be called right from the beginning
	static void logMessage(MessageType type, const char* message);
	//Function called to enable/disable output messages. Used when RLSimion outputs its requirements
	static void enableLogMessages(bool enable);

protected:
	friend class Experiment;
	//METHODS CALLED FROM Experiment
	//called to log episodes
	void firstEpisode();
	void lastEpisode();
	//called to log steps
	void firstStep();
	void lastStep();
	void timestep(State* s, Action* a, State* s_p,Reward* r);
};