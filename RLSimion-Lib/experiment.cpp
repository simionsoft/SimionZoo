#include "stdafx.h"
#include "experiment.h"
#include "parameters.h"
#include "logger.h"
#include "world.h"
#include "globals.h"
#include "stats.h"



CExperimentTime& CExperimentTime::operator=(CExperimentTime& exp)
{
	m_step = exp.m_step;
	m_episodeIndex = exp.m_episodeIndex;
}

bool CExperimentTime::operator==(CExperimentTime& exp)
{
	return m_step == exp.m_step && m_episodeIndex == exp.m_episodeIndex;
}

double CExperimentProgress::getExperimentProgress()
{
	double progress = ((double)m_step - 1 + (m_episodeIndex - 1)*m_numSteps)
		/ ((double)m_numSteps*m_totalNumEpisodes - 1);
	return progress;
}

double CExperimentProgress::getEpisodeProgress()
{
	double progress = ((double)m_step - 1)	/ ((double)m_numSteps - 1);
	return progress;
}



const char* CExperimentProgress::getProgressString()
{
	sprintf_s(m_progressMsg, MAX_PROGRESS_MSG_LEN, "Episode: %d/%d Step %d/%d (%.2f%%)"
		, getEpisodeIndex(), getTotalNumEpisodes()
		, getStep(), getNumSteps()
		, getExperimentProgress()*100.0);
	return m_progressMsg;
}

void CExperimentProgress::setNumEpisodes(unsigned int numTrainingEpisodes, unsigned int evalFreq)
{
	m_numTrainingEpisodes = numTrainingEpisodes;
	m_evalFreq = std::max(numTrainingEpisodes,evalFreq);
	if (m_evalFreq != 0)
	{
		m_numEvaluationEpisodes = 1 + numTrainingEpisodes / m_evalFreq;
		m_totalNumEpisodes = numTrainingEpisodes + m_numEvaluationEpisodes;
	}
	else
	{
		m_numEvaluationEpisodes = 0;
		m_totalNumEpisodes = numTrainingEpisodes;
	}
}

bool CExperimentProgress::isEvaluationEpisode()
{
	if (m_evalFreq > 0)
	{
		return (m_episodeIndex - 1) % (m_evalFreq + 1) == 0;
	}
	return false;
}

void CExperimentProgress::reset()
{
	m_trainingEpisodeIndex= 0; //[1..m_numTrainingEpisodes]
	m_evalEpisodeIndex= 0; //[1..1+m_numTrainingEpisodes/ evalFreq]
	m_episodeIndex= 0; //[1..g_numEpisodes]
	m_step= 0; //]1..g_numStepsPerEpisode]
}


void CExperimentProgress::nextStep()
{
	if (m_step<m_numSteps)
		m_step ++;

	else m_step = 0;
}

void CExperimentProgress::nextEpisode()
{
	m_episodeIndex++;
	if (isEvaluationEpisode()) m_evalEpisodeIndex++;
	else m_trainingEpisodeIndex++;
}

bool CExperimentProgress::isFirstEpisode()
{
	if (isEvaluationEpisode()) return m_evalEpisodeIndex == 1;
	return m_trainingEpisodeIndex == 1;
}

bool CExperimentProgress::isLastEpisode()
{
	if (isEvaluationEpisode()) return m_evalEpisodeIndex == m_numEvaluationEpisodes;
	return m_trainingEpisodeIndex == m_numTrainingEpisodes;
}

CExperiment::~CExperiment()
{
}

CExperiment::CExperiment(CParameters* pParameters)
{
	if (pParameters)
	{
		m_randomSeed = pParameters->getConstInteger("Random-Seed", 1);

		m_expProgress.setNumEpisodes(pParameters->getConstInteger("Num-Episodes", 1)
			, pParameters->getConstInteger("Eval-Freq", 0));
		m_expProgress.setNumSteps((unsigned int)
			(pParameters->getConstDouble("Episode-Length", 1.0) / RLSimion::g_pWorld->getDT()));
		m_expProgress.reset();


		m_pLogger = new CLogger(pParameters->getChild("Log"));
	}
	else
	{
		m_randomSeed = 0;
		m_expProgress.setNumEpisodes(0,0);
		m_expProgress.setNumSteps(0);
	}
	srand(m_randomSeed);
}




void CExperiment::timestep(CState* s, CAction* a, CState* s_p, CReward* r)
{
	bool evalEpisode = m_expProgress.isEvaluationEpisode();
	if (m_expProgress.isFirstEpisode() && m_expProgress.isFirstStep())
		m_pLogger->firstEpisode(evalEpisode);

	if (m_expProgress.isFirstStep())
		m_pLogger->firstStep(evalEpisode);

	//update stats
	//output step-stats
	m_pLogger->timestep(evalEpisode);

	if (m_expProgress.isLastStep())
		m_pLogger->lastStep(evalEpisode);

	if (m_expProgress.isLastEpisode() && m_expProgress.isLastStep())
		m_pLogger->lastEpisode(evalEpisode);
}