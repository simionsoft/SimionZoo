#pragma once

class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;
typedef CNamedVarSet CReward;
class CParameters;




class CLogger;

#define MAX_PROGRESS_MSG_LEN 1024

class CExperimentProgress
{
	unsigned int m_episode; //[1..g_numEpisodes]
	unsigned int m_step; //]1..g_numStepsPerEpisode]

	unsigned int m_numEpisodes;
	unsigned int m_numSteps;

	char m_progressMsg[MAX_PROGRESS_MSG_LEN];
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
	bool isValidStep(){ return m_step > 0 && m_step <= m_numSteps; }
	void incStep(){ m_step++; }
	bool isFirstEpisode(){ return m_episode == 1; }
	bool isLastEpisode(){ return m_episode == m_numEpisodes; }
	bool isValidEpisode(){ return m_episode > 0 && m_episode <= m_numEpisodes; }
	void incEpisode(){ m_episode++; }

	unsigned int getNumEpisodes(){ return m_numEpisodes; }
	unsigned int getNumSteps(){ return m_numSteps; }
	void setNumEpisodes(unsigned int numEpisodes){ m_numEpisodes = numEpisodes; }
	void setNumSteps(unsigned int numSteps){ m_numSteps = numSteps; }

	double getExperimentProgress(); //[0,1]
	double getEpisodeProgress(); //[0,1]

	const char* getProgressString();
};

class CExperiment
{
	bool isCurrentEpisodeLogged();
public:
	CExperiment(CParameters* pParameters);
	~CExperiment();

	CExperimentProgress m_expProgress;
	CLogger* m_pLogger;

	unsigned int m_randomSeed;
	unsigned int m_evalFreq; //in episodes

	bool isEvaluationEpisode();

	void timestep(CState *s, CAction *a,CState *s_p, CReward* pReward);
};