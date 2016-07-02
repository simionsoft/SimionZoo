#pragma once

class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;
typedef CNamedVarSet CReward;
class CParameters;
#include <vector>
class CStats;
class CTimer;
class CFilePathList;

enum MessageType {Progress,Evaluation,Info,Warning, Error};
enum MessageOutputMode {Console,NamedPipe};
enum Output{Pipe,LogFile};

class CLogger
{
	static const int MAX_FILENAME_LENGTH = 1024;
	static const int BUFFER_SIZE = 10000;
	
	//log file
	char* m_pFullLogFilename;
	static void *m_hLogFile;

	bool m_bLogEvaluationEpisodes;
	bool m_bLogTrainingEpisodes;
	bool m_bLogEvaluationExperiment;
	bool m_bLogTrainingExperiment;

	double m_logFreq; //in seconds: time between file logs

	CTimer *m_pEpisodeTimer;
	CTimer *m_pExperimentTimer;

	double m_episodeRewardSum;

	double m_lastLogSimulationT;

	void openLogFile(const char* fullLogFilename);
	void closeLogFile();

	char* getExperimentTypeName(bool evalEpisode);

	void logStepData(unsigned int stepIndex, CState* s, CAction* a, CState* s_p, CReward* r);
	void logNamedVarSetToBuffer(char* pOutBuffer,CNamedVarSet* pNamedVarSet, const char* id);
	void logStatsToBuffer(char* pOutBuffer);

	static void writeLogBuffer(Output output, const char* pBuffer);

	//stats
	std::vector<CStats*> m_stats;
public:
	CLogger();
	virtual ~CLogger();
	void init(CParameters* pParameters);

	//returns whether a specific type of episode is going to be logged
	bool isEpisodeTypeLogged(bool evaluation);
	bool isExperimentTypeLogged(bool evaluation);

	//METHODS CALLED FROM ANY CLASS
	void addVarToStats(const char* key, const char* subkey, double* variable);
	void addVarToStats(const char* key, const char* subkey, int* variable);
	void addVarToStats(const char* key, const char* subkey, unsigned int* variable);
	void addVarSetToStats(const char* key, CNamedVarSet* varset);

	void setLogDirectory(const char* xmlFilePath);

	static MessageOutputMode m_messageOutputMode;
	static void* m_outputPipe;
	static void createOutputPipe(const char* pipeName);
	static void closeOutputPipe();

	//Function called to report progress and error messages
	//static so that it can be called right from the beginning
	static void logMessage(MessageType type, const char* message);


protected:
	friend class CExperiment;
	//METHODS CALLED FROM CExperiment
	//called to log episodes
	void firstEpisode(bool evalEpisode);
	void lastEpisode(bool evalEpisode);
	//called to log steps
	void firstStep(bool evalEpisode, unsigned int episodeIndex);
	void lastStep(bool evalEpisode, unsigned int episodeIndex, unsigned int numEpisodes= 0, unsigned int numSteps= 0);
	void timestep(bool evalEpisode, unsigned int episodeIndex,CState* s, CAction* a, CState* s_p,CReward* r);
};