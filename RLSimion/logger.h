#pragma once

#include <vector>
#include "parameters.h"
#include "../tools/WindowsUtils/NamedPipe.h"
#include "stats.h"

class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;
typedef CNamedVarSet CReward;
class CConfigNode;
class CDescriptor;
class CTimer;

enum MessageType {Progress,Evaluation,Info,Warning, Error};
enum MessageOutputMode {Console,NamedPipe};

class CLogger
{
	static const int MAX_FILENAME_LENGTH = 1024;
	static const int BUFFER_SIZE = 10000;

	//log file
	string m_outputLogDescriptor;
	string m_outputLogBinary;
	static FILE *m_logFile;

	BOOL_PARAM m_bLogEvaluationEpisodes;
	BOOL_PARAM m_bLogTrainingEpisodes;

	DOUBLE_PARAM m_logFreq; //in seconds: time between file logs

	CTimer *m_pEpisodeTimer;
	CTimer *m_pExperimentTimer;

	double m_episodeRewardSum;

	double m_lastLogSimulationT;

	void openLogFile(const char* fullLogFilename);
	void closeLogFile();

	static void writeLogBuffer(const char* pBuffer, int numBytes);

	void writeLogFileXMLDescriptor(const char* filename);

	void writeNamedVarSetDescriptorToBuffer(char* buffer, const char* id, const CDescriptor* pNamedVarSet);
	void writeStatDescriptorToBuffer(char* buffer);
	void writeEpisodeTypesToBuffer(char* buffer);

	void writeExperimentHeader();
	void writeEpisodeHeader();
	void writeEpisodeEndHeader();
	int writeStepHeaderToBuffer(char* buffer, int offset);
	void writeStepData(CState* s, CAction* a, CState* s_p, CReward* r);
	int writeNamedVarSetToBuffer(char* buffer, int offset, const CNamedVarSet* pNamedVarSet);
	int writeStatsToBuffer(char* buffer, int offset);

	//stats
	std::vector<IStats *> m_stats;
public:
	static const unsigned int BIN_FILE_VERSION = 2;

	CLogger(CConfigNode* pParameters);
	CLogger() = default;
	virtual ~CLogger();

	//returns whether a specific type of episode is going to be logged
	bool isEpisodeTypeLogged(bool evaluation);

	//METHODS CALLED FROM ANY CLASS
	template <typename T>
	void addVarToStats(string key, string subkey, T& variable)
	{
		m_stats.push_back(new CStats<T>(key, subkey, variable));
	}
	void addVarSetToStats(const char* key, CNamedVarSet* varset);

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
	friend class CExperiment;
	//METHODS CALLED FROM CExperiment
	//called to log episodes
	void firstEpisode();
	void lastEpisode();
	//called to log steps
	void firstStep();
	void lastStep();
	void timestep(CState* s, CAction* a, CState* s_p,CReward* r);
};