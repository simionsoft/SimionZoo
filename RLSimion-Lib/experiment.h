#pragma once
#include "parameters.h"
class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;
typedef CNamedVarSet CReward;
class CConfigNode;
class CTimer;

#define MAX_PROGRESS_MSG_LEN 1024

//utility class to get a experiment time reference
//for example, we can use this reference to know if some feature vector has already been calculated in this time-step
class CExperimentTime
{
public:
	unsigned int m_episodeIndex; //[1..g_numEpisodes]
	unsigned int m_step; //]1..g_numStepsPerEpisode]

	CExperimentTime(unsigned int episodeIndex, unsigned int step){ m_episodeIndex = episodeIndex; m_step = step; }

	CExperimentTime& operator=(CExperimentTime& exp);
	bool operator==(CExperimentTime& exp);
};



class CExperiment
{
	unsigned int m_episodeIndex; //[1..g_numEpisodes]
	unsigned int m_step; //]1..g_numStepsPerEpisode]
	unsigned int m_trainingEpisodeIndex; //[1..m_numTrainingEpisodes]
	unsigned int m_evalEpisodeIndex; //[1..1+m_numTrainingEpisodes/ evalFreq]

	INT_PARAM m_randomSeed;

	//episode stuff
	DOUBLE_PARAM m_episodeLength;
	unsigned int m_totalNumEpisodes;
	INT_PARAM m_numTrainingEpisodes;
	unsigned int m_numEvaluationEpisodes;
	INT_PARAM m_evalFreq;
	//steps
	unsigned int m_numSteps;
	bool m_bTerminalState;

	DOUBLE_PARAM m_progUpdateFreq; //in seconds: time between progress updates
	CTimer* m_pProgressTimer;

	char m_progressMsg[MAX_PROGRESS_MSG_LEN];
	void reset();
public:
	CExperiment(CConfigNode* pParameters);
	CExperiment() = default;
	virtual ~CExperiment();

	void getExperimentTime(CExperimentTime& ref) { ref.m_episodeIndex = m_episodeIndex; ref.m_step = m_step; }


	//STEP
	bool isValidStep();
	unsigned int getStep(){ return m_step; }
	bool isFirstStep(){ return m_step == 1; }
	bool isLastStep(){ return (m_bTerminalState || m_step == m_numSteps); }
	void setTerminalState(){ m_bTerminalState = true; }
	void nextStep();

	//EPISODES
	
	//Returns the expected episode length in seconds (simulation time)
	double getEpisodeLength(){ return m_episodeLength.get(); }//seconds
	unsigned int getEpisodeIndex(){ return m_episodeIndex; }
	unsigned int getRelativeEpisodeIndex(){ if (!isEvaluationEpisode()) return m_trainingEpisodeIndex; return m_evalEpisodeIndex; }
	unsigned int getTrainingEpisodeIndex(){ return m_trainingEpisodeIndex; }
	unsigned int getEvaluationEpisodeIndex(){ return m_evalEpisodeIndex; }
	void nextEpisode();
	//true if is the first evaluation episode or the first training episode
	bool isFirstEpisode();
	//true if is the last evaluation episode or the last training episode
	bool isLastEpisode();
	bool isValidEpisode(){ return m_episodeIndex > 0 && m_episodeIndex <= m_totalNumEpisodes; }

	bool isEvaluationEpisode();

	unsigned int getNumEvaluationEpisodes(){ return m_numEvaluationEpisodes; }
	unsigned int getNumTrainingEpisodes(){ return m_numTrainingEpisodes.get(); }
	unsigned int getTotalNumEpisodes(){ return m_totalNumEpisodes; }
	unsigned int getNumSteps(){ return m_numSteps; }
	void setNumSteps(unsigned int numSteps){ m_numSteps = numSteps; }

	double getExperimentProgress(); //[0,1]
	double getEpisodeProgress(); //[0,1]

	const char* getProgressString();




	void timestep(CState *s, CAction *a,CState *s_p, CReward* pReward);
};