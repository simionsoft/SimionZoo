#pragma once

class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;
typedef CNamedVarSet CReward;
class CConfigNode;
class CDescriptor;
#include <vector>
#include "parameters.h"
#include "../tools/WindowsUtils/NamedPipe.h"
class CStats;
class CTimer;
class CFilePathList;

enum MessageType {Progress,Evaluation,Info,Warning, Error};
enum MessageOutputMode {Console,NamedPipe};

class CLogger
{
	static const int MAX_FILENAME_LENGTH = 1024;
	static const int BUFFER_SIZE = 10000;

	
	//log file
	char m_outputDir[MAX_FILENAME_LENGTH];
	char* m_pFullLogFilename;
	static void *m_hLogFile;

	BOOL_PARAM m_bLogEvaluationEpisodes;
	BOOL_PARAM m_bLogTrainingEpisodes;

	DOUBLE_PARAM m_logFreq; //in seconds: time between file logs

	CTimer *m_pEpisodeTimer;
	CTimer *m_pExperimentTimer;

	double m_episodeRewardSum;

	double m_lastLogSimulationT;

	void* openLogFile(const char* fullLogFilename);
	void closeLogFile(void* fileHandle);

	static void writeLogBuffer(void* fileHandle, const char* pBuffer, int numBytes);

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
	std::vector<CStats*> m_stats;
public:
	static const unsigned int BIN_FILE_VERSION = 1;


	CLogger(CConfigNode* pParameters);
	CLogger() = default;
	virtual ~CLogger();

	//returns whether a specific type of episode is going to be logged
	bool isEpisodeTypeLogged(bool evaluation);

	//METHODS CALLED FROM ANY CLASS
	void addVarToStats(const char* key, const char* subkey, double* variable);
	void addVarToStats(const char* key, const char* subkey, int* variable);
	void addVarToStats(const char* key, const char* subkey, unsigned int* variable);
	void addVarSetToStats(const char* key, CNamedVarSet* varset);

	void setLogDirectory(const char* xmlFilePath);

	static MessageOutputMode m_messageOutputMode;

	static CNamedPipeClient m_outputPipe;

	//Function called to report progress and error messages
	//static so that it can be called right from the beginning
	static void logMessage(MessageType type, const char* message);


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