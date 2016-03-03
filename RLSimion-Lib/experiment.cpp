#include "stdafx.h"
#include "experiment.h"
#include "parameters.h"
#include "logger.h"
#include "world.h"
#include "globals.h"
#include "stats.h"

double CExperimentProgress::getExperimentProgress()
{
	double progress = ((double)m_step - 1 + (m_episode - 1)*m_numSteps)
		/ ((double)m_numSteps*m_numEpisodes - 1);
	return progress;
}

double CExperimentProgress::getEpisodeProgress()
{
	double progress = ((double)m_step - 1)	/ ((double)m_numSteps - 1);
	return progress;
}

CExperimentProgress& CExperimentProgress::operator=(CExperimentProgress& exp)
{
	m_step = exp.getStep();
	m_episode = exp.getEpisode();
	m_numEpisodes = exp.getNumEpisodes();
	m_numSteps = exp.getNumSteps();
	return *this;
}
bool CExperimentProgress::operator==(CExperimentProgress& exp)
{
	return m_step == exp.getStep() && m_episode == exp.getEpisode();
}




CExperiment::~CExperiment()
{
}

CExperiment::CExperiment(CParameters* pParameters)
{
	if (pParameters)
	{
		m_randomSeed = pParameters->getConstInteger("Random-Seed", 1);

		m_expProgress.setNumEpisodes(pParameters->getConstInteger("Num-Episodes", 1));
		m_expProgress.setNumSteps((unsigned int)
			(pParameters->getConstDouble("Episode-Length", 1.0) / RLSimion::g_pWorld->getDT()));

		m_evalFreq = pParameters->getConstInteger("Eval-Freq", 0);

		m_pLogger = new CLogger(pParameters->getChild("Log"));
	}
	else
	{
		m_randomSeed = 0;
		m_expProgress.setNumEpisodes(0);
		m_expProgress.setNumSteps(0);
		m_evalFreq = 0;
	}
	srand(m_randomSeed);
}

bool CExperiment::isEvaluationEpisode()
{
	if (m_evalFreq==0) return false;
	if (m_expProgress.getEpisode() == 1 || (m_expProgress.getEpisode() % m_evalFreq == 0))
		return true;
	return false;
}



void CExperiment::timestep(CState* s, CAction* a, CState* s_p, CReward* r)
{
	bool evalEpisode = isEvaluationEpisode();
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