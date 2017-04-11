#include "stdafx.h"
#include "experiment.h"
#include "config.h"
#include "logger.h"
#include "world.h"
#include "stats.h"
#include "timer.h"
#include "app.h"

CExperimentTime& CExperimentTime::operator=(CExperimentTime& exp)
{
	m_step = exp.m_step;
	m_episodeIndex = exp.m_episodeIndex;

	return *this;
}

bool CExperimentTime::operator==(CExperimentTime& exp)
{
	return m_step == exp.m_step && m_episodeIndex == exp.m_episodeIndex;
}

double CExperiment::getExperimentProgress()
{
	double progress = ((double)m_step - 1 + (m_episodeIndex - 1)*m_numSteps)
		/ ((double)m_numSteps*m_totalNumEpisodes - 1);
	return progress;
}

double CExperiment::getTrainingProgress()
{
	if (m_trainingEpisodeIndex == 0) //not a single training episode yet
		return 0.0;
	double progress = ((double)m_step - 1 + (m_trainingEpisodeIndex - 1)*m_numSteps)
		/ ((double)m_numSteps*m_numTrainingEpisodes.get() - 1);
	return progress;
}

double CExperiment::getEpisodeProgress()
{
	double progress = ((double)m_step - 1)	/ ((double)m_numSteps - 1);
	return progress;
}

const char* CExperiment::getProgressString()
{
	sprintf_s(m_progressMsg, MAX_PROGRESS_MSG_LEN, "Episode: %d/%d Step %d/%d (%.2f%%)"
		, getEpisodeIndex(), getTotalNumEpisodes()
		, getStep(), getNumSteps()
		, getExperimentProgress()*100.0);
	return m_progressMsg;
}

void CExperiment::setEpisodeLength(double length)
{
	m_episodeLength.set(length);
	reset();
}
void CExperiment::setEvaluationFreq(int evalFreq)
{
	m_evalFreq.set(evalFreq);
	reset();
}
void CExperiment::setNumEpisodesPerEvaluation(int numEpisodes)
{
	m_numEpisodesPerEvaluation = numEpisodes;
	reset();
}
void CExperiment::setNumTrainingEpisodes(int numEpisodes)
{
	m_numTrainingEpisodes.set(numEpisodes);
	reset();
}
void CExperiment::setNumSteps(int numSteps)
{
	m_numSteps = numSteps;
	reset();
}

bool CExperiment::isEvaluationEpisode()
{
	if (m_evalFreq.get() > 0)
	{
		unsigned int episodeInEvalTrainingCycle = (m_episodeIndex-1)
			% ( m_numEpisodesPerEvaluation + m_evalFreq.get());
		return episodeInEvalTrainingCycle < m_numEpisodesPerEvaluation;
	//	return (m_episodeIndex - 1) % (m_evalFreq.getSample() + 1) == 0;
	}
	return true;
}

unsigned int CExperiment::getEpisodeInEvaluationIndex()
{
	//the index of the last evaluation (1-based index)
	return 1+( (m_evalEpisodeIndex-1) % m_numEpisodesPerEvaluation);
}
unsigned int CExperiment::getEvaluationIndex()
{
	//the last episode within last evaluation
	return 1+( (m_evalEpisodeIndex-1) / m_numEpisodesPerEvaluation);
}
unsigned int CExperiment::getRelativeEpisodeIndex()
{
	if (!isEvaluationEpisode())
		return m_trainingEpisodeIndex;
	return getEvaluationIndex();
}

void CExperiment::reset()
{
	m_trainingEpisodeIndex= 0; //[1..m_numTrainingEpisodes]
	m_evalEpisodeIndex= 0; //[1..1+m_numTrainingEpisodes/ evalFreq]
	m_episodeIndex= 0; //[1..g_numEpisodes]
	m_step= 0; //]1..g_numStepsPerEpisode]
	m_experimentStep = 0;
	m_bTerminalState = false;

	//calculate the number of episodes
	if (m_evalFreq.get() != 0)
	{
		if (m_numTrainingEpisodes.get() > 1)
		{	//general case: training and evaluation
			m_numEvaluations = 1 + m_numTrainingEpisodes.get() / m_evalFreq.get();
			m_totalNumEpisodes = m_numTrainingEpisodes.get()
				+ m_numEvaluations*m_numEpisodesPerEvaluation;
		}
		else
		{	//no training, just one evaluation
			m_numEvaluations = 1;
			m_numTrainingEpisodes.set(0);//no training
			m_totalNumEpisodes = m_numEvaluations*m_numEpisodesPerEvaluation;
		}
	}
	else
	{	//only training
		m_numEvaluations = 0;
		m_totalNumEpisodes = (unsigned int)m_numTrainingEpisodes.get();
	}
	//number of steps
	if (CSimionApp::get() != nullptr && CSimionApp::get()->pWorld.ptr() != nullptr 
		&& CSimionApp::get()->pWorld->getDT()>0.0)
		m_numSteps= ((unsigned int)(m_episodeLength.get() / CSimionApp::get()->pWorld->getDT()));
	else
	{
		CLogger::logMessage(MessageType::Warning, "CExperiment: Failed to get DT. m_numSteps will be used instead");
	}
}


void CExperiment::nextStep()
{
	m_experimentStep++;

	if (!m_bTerminalState && m_step<m_numSteps)
		m_step ++;
	else m_step = 0;
}

bool CExperiment::isValidStep()
{
	return !m_bTerminalState && m_step > 0 && m_step <= m_numSteps;
}

bool CExperiment::isValidEpisode()
{
	return m_episodeIndex > 0 && m_episodeIndex <= m_totalNumEpisodes;
}

void CExperiment::nextEpisode()
{
	//reset the terminal state flag at the beginning of every episode
	m_bTerminalState = false;

	m_episodeIndex++;
	if (isEvaluationEpisode())
	{
		m_evalEpisodeIndex++;
	}
	else
	{
		m_trainingEpisodeIndex++;
	}
}

bool CExperiment::isFirstEpisode()
{
	return m_episodeIndex == 1;
}

bool CExperiment::isLastEpisode()
{
	if (isEvaluationEpisode()) 
		return m_evalEpisodeIndex == m_numEvaluations*m_numEpisodesPerEvaluation;
	return m_trainingEpisodeIndex == m_numTrainingEpisodes.get();
}

CExperiment::~CExperiment()
{
	if (m_pProgressTimer)
		delete m_pProgressTimer;
}

CExperiment::CExperiment(CConfigNode* pConfigNode)
{
	if (!pConfigNode) return;

	m_progUpdateFreq = DOUBLE_PARAM(pConfigNode, "Progress-Update-Freq", "Progress update frequency (seconds)",1.0);
	m_randomSeed = INT_PARAM(pConfigNode, "Random-Seed","Random seed used to generate random sequences of numbers",1);

	m_numTrainingEpisodes = INT_PARAM(pConfigNode, "Num-Episodes","Number of episodes",1000);
	m_evalFreq = INT_PARAM(pConfigNode,"Eval-Freq", "Evaluation frequency (in episodes)",10);

	m_episodeLength = DOUBLE_PARAM(pConfigNode, "Episode-Length", "Length of an episode(seconds)",10.0);

	reset();	//calculate all the variables not given as parameters
				//and reset counters

	m_pProgressTimer = new CTimer();

	srand((unsigned int)m_randomSeed.get());
}


void CExperiment::timestep(CState* s, CAction* a, CState* s_p, CReward* r)
{
	char msg[1024];


	double time = m_pProgressTimer->getElapsedTime();//(double)(currentCounter - m_lastProgressReportCounter) / (double)m_counterFreq;

	if (time>m_progUpdateFreq.get() || (isLastStep() && isLastEpisode()))
	{
		sprintf_s(msg, 1024, "%f", CSimionApp::get()->pExperiment->getExperimentProgress()*100.0);
		CLogger::logMessage(Progress, msg);
		m_pProgressTimer->startTimer();
	}

	bool evalEpisode = isEvaluationEpisode();
	if (isFirstEpisode() && isFirstStep())
		CSimionApp::get()->pLogger->firstEpisode();

	unsigned int episodeIndex = getRelativeEpisodeIndex();
	if (isFirstStep())
		CSimionApp::get()->pLogger->firstStep();

	//update stats
	//output step-stats
	CSimionApp::get()->pLogger->timestep(s, a, s_p, r);

	if (isLastStep())
		CSimionApp::get()->pLogger->lastStep();

	if (isLastEpisode() && (isLastStep()))
		CSimionApp::get()->pLogger->lastEpisode();
}