#pragma once

class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;
typedef CNamedVarSet CReward;


class tinyxml2::XMLElement;

#include <vector>
class CStats;


class CLogger
{
	char *m_outputDir;
	char *m_filePrefix;

	bool m_bLogEvaluationEpisodes;
	bool m_bLogTrainingEpisodes;
	bool m_bLogEvaluationExperiment;
	bool m_bLogTrainingExperiment;

	double m_logFreq; //in seconds: time between file logs
	double m_progUpdateFreq; //in seconds: time between progress updates

	//__int64 m_lastCounter;




	////regular log files
	//void openEpisodeLogFile();
	//void writeEpisodeStep(CState *s, CAction *a, CState *s_p, CReward *pReward);
	//void writeEpisodeLogFileHeader(CState *s, CAction *a, CReward *pReward);

	////summary files: avg reward of evaluation episodes
	//void writeEpisodeSummary();

	__int64 m_counterFreq;
	__int64 m_episodeStartCounter;
	__int64 m_experimentStartCounter;
	__int64 m_lastProgressReportCounter;
	double m_lastLogSimulationT;

	bool logCurrentEpisode(bool evalEpisode);
	bool logCurrentEpisodeInExperiment(bool evalEpisode);

	void getLogFilename(char* buffer, int bufferSize, bool episodeLog, bool evaluation);
	//opens the experiment log file
	//if create=true, the output file is created ("w" mode), writes the header and closes the file. Returns 0
	//otherwise, the file is opened to append the new info ("a" mode). Returns the file just opened
	//evalEpisode determines whether it is evaluation or training log data
	FILE* openLogFile(bool create, bool episodeLog, bool evalEpisode);
	void writeLogFileHeader(FILE* pFile, bool episodeLog, bool evalEpisode);

	//adds current experiment data to the experiment log file
	void writeExperimentLogData(bool evalEpisode);
	//adds current experiment data to the experiment log file
	void writeEpisodeLogData(bool evalEpisode);

	//stats
	std::vector<CStats*> m_stats;
public:
	CLogger(tinyxml2::XMLElement* pParameters);
	~CLogger();

	void addVarToStats(const char* key, const char* subkey, double* variable);
	void addVarToStats(const char* key, const char* subkey, int* variable);
	void addVarToStats(const char* key, const char* subkey, unsigned int* variable);
	void addVarSetToStats(const char* key, CNamedVarSet* varset);


	//called to log episodes
	void firstEpisode(bool evalEpisode);
	void lastEpisode(bool evalEpisode);
	//called to log steps
	void firstStep(bool evalEpisode);
	void lastStep(bool evalEpisode);
	void timestep(bool evalEpisode);
};