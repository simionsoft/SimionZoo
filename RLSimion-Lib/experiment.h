#pragma once

class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;
class CParameters;

#define MAX_FILENAME_LENGTH 512

class CExperiment
{
	char m_outputDir[MAX_FILENAME_LENGTH];
	char m_filePrefix[MAX_FILENAME_LENGTH];

	void *m_pFile;

	bool m_bLogEvaluationEpisodes;
	bool m_bLogTrainingEpisodes;
	bool m_bLogEvaluationSummary;
	unsigned int m_evalFreq; //in episodes
	double m_logFreq; //in seconds

	__int64 m_lastCounter;
	__int64 m_counterFreq;

	double m_lastLogTime;
	double m_episodeRewards;
	double m_lastEvaluationAvgReward;
	
	//regular log files
	void openEpisodeLogFile();
	void writeEpisodeStep(CState *s, CAction *a, CState *s_p, double r);
	void writeEpisodeLogFileHeader(CState *s, CAction *a);

	//summary files: avg reward of evaluation episodes
	void writeEpisodeSummary();

public:
	
	unsigned int m_numEpisodes;
	unsigned int m_numSteps;
	unsigned int m_episode; //[1..g_numEpisodes]
	unsigned int m_step; //]1..g_numStepsPerEpisode]
	unsigned int m_randomSeed;

	CExperiment(CParameters* pParameters);
	~CExperiment();

	bool isCurrentEpisodeLogged();
	bool isEvaluationEpisode();
	double getProgress();
	double getCurrentAvgReward();

	void logStep(CState *s, CAction *a,CState *s_p, double r);
};