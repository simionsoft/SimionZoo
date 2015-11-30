#pragma once

class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;
class CParameters;

#define MAX_FILENAME_LENGTH 512

class CExperimentProgress
{
	unsigned int m_episode; //[1..g_numEpisodes]
	unsigned int m_step; //]1..g_numStepsPerEpisode]

	unsigned int m_numEpisodes;
	unsigned int m_numSteps;
public:
	CExperimentProgress(){ m_episode = 0; m_step = 0; }
	~CExperimentProgress(){}

	CExperimentProgress& operator=(CExperimentProgress& exp);
	bool operator==(CExperimentProgress& exp);

	unsigned int getEpisode(){ return m_episode; }
	unsigned int getStep(){ return m_step; }
	bool isFirstStep(){ return m_step == 1; }
	bool isLastStep(){ return m_step == m_numSteps; }
	void setEpisode(unsigned int episode){ m_episode = episode; }
	void setStep(unsigned int step){ m_step = step; }
	bool isValidStep(){ return m_step > 0 && m_step < m_numSteps; }
	void incStep(){ m_step++; }
	bool isFirstEpisode(){ return m_episode == 1; }
	bool isLastEpisode(){ return m_episode == m_numEpisodes; }
	bool isValidEpisode(){ return m_episode > 0 && m_episode < m_numEpisodes; }
	void incEpisode(){ m_episode++; }

	unsigned int getNumEpisodes(){ return m_numEpisodes; }
	unsigned int getNumSteps(){ return m_numSteps; }
	void setNumEpisodes(unsigned int numEpisodes){ m_numEpisodes = numEpisodes; }
	void setNumSteps(unsigned int numSteps){ m_numSteps = numSteps; }

	double getExperimentProgress(); //[0,1]
	double getEpisodeProgress(); //[0,1]
};

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
	
	CExperimentProgress m_expProgress;

	unsigned int m_randomSeed;

	CExperiment(CParameters* pParameters);
	~CExperiment();

	bool isCurrentEpisodeLogged();
	bool isEvaluationEpisode();
	double getCurrentAvgReward();

	void logStep(CState *s, CAction *a,CState *s_p, double r);
};