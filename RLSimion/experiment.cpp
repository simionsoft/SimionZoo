#include "experiment.h"
#include "config.h"
#include "logger.h"
#include "worlds/world.h"
#include "stats.h"
#include "../tools/System/Timer.h"
#include "../tools/System/CrossPlatform.h"
#include "app.h"

ExperimentTime& ExperimentTime::operator=(ExperimentTime& exp)
{
	m_step = exp.m_step;
	m_episodeIndex = exp.m_episodeIndex;

	return *this;
}

bool ExperimentTime::operator==(ExperimentTime& exp)
{
	return m_step == exp.m_step && m_episodeIndex == exp.m_episodeIndex;
}

double Experiment::getExperimentProgress()
{
	double progress = ((double)m_step - 1 + (m_episodeIndex - 1)*m_numSteps)
		/ ((double)m_numSteps*m_totalNumEpisodes - 1);
	return progress;
}

double Experiment::getTrainingProgress()
{
	if (m_trainingEpisodeIndex == 0) //not a single training episode yet
		return 0.0;
	double progress = ((double)m_step - 1 + (m_trainingEpisodeIndex - 1)*m_numSteps)
		/ ((double)m_numSteps*m_numTrainingEpisodes.get() - 1);
	return progress;
}

double Experiment::getEpisodeProgress()
{
	double progress = ((double)m_step - 1) / ((double)m_numSteps - 1);
	return progress;
}

const char* Experiment::getProgressString()
{
	CrossPlatform::sprintf_s(m_progressMsg, MAX_PROGRESS_MSG_LEN, "Episode: %d/%d Step %d/%d (%.2f%%)"
		, getEpisodeIndex(), getTotalNumEpisodes()
		, getStep(), getNumSteps()
		, getExperimentProgress()*100.0);
	return m_progressMsg;
}

void Experiment::setEpisodeLength(double length)
{
	m_episodeLength.set(length);
	reset();
}
void Experiment::setEvaluationFreq(int evalFreq)
{
	m_evalFreq.set(evalFreq);
	reset();
}
void Experiment::setNumEpisodesPerEvaluation(int numEpisodes)
{
	m_numEpisodesPerEvaluation = numEpisodes;
	reset();
}
void Experiment::setNumTrainingEpisodes(int numEpisodes)
{
	m_numTrainingEpisodes.set(numEpisodes);
	reset();
}
void Experiment::setNumSteps(int numSteps)
{
	m_numSteps = numSteps;
	reset();
}

bool Experiment::isEvaluationEpisode()
{
	if (m_evalFreq.get() > 0)
	{
		unsigned int episodeInEvalTrainingCycle = (m_episodeIndex - 1)
			% (m_numEpisodesPerEvaluation + m_evalFreq.get());
		return episodeInEvalTrainingCycle < m_numEpisodesPerEvaluation;
	}
	return true;
}

unsigned int Experiment::getEpisodeInEvaluationIndex()
{
	//the index of the last evaluation (1-based index)
	return 1 + ((m_evalEpisodeIndex - 1) % m_numEpisodesPerEvaluation);
}
unsigned int Experiment::getEvaluationIndex()
{
	//the last episode within last evaluation
	return 1 + ((m_evalEpisodeIndex - 1) / m_numEpisodesPerEvaluation);
}
unsigned int Experiment::getRelativeEpisodeIndex()
{
	if (!isEvaluationEpisode())
		return m_trainingEpisodeIndex;
	return getEvaluationIndex();
}

void Experiment::reset()
{
	m_trainingEpisodeIndex = 0; //[1..m_numTrainingEpisodes]
	m_evalEpisodeIndex = 0; //[1..1+m_numTrainingEpisodes/ evalFreq]
	m_episodeIndex = 0; //[1..g_numEpisodes]
	m_step = 0; //]1..g_numStepsPerEpisode]
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
	if (SimionApp::get() != nullptr && SimionApp::get()->pWorld.ptr() != nullptr
		&& SimionApp::get()->pWorld->getDT() > 0.0)
		m_numSteps = ((unsigned int)(m_episodeLength.get() / SimionApp::get()->pWorld->getDT()));
	else
	{
		Logger::logMessage(MessageType::Warning, "Experiment: Failed to get DT. m_numSteps will be used instead");
	}
}


void Experiment::nextStep()
{
	m_experimentStep++;

	if (!m_bTerminalState && m_step < m_numSteps)
		m_step++;
	else m_step = 0;
}

bool Experiment::isValidStep()
{
	return !m_bTerminalState && m_step > 0 && m_step <= m_numSteps;
}

bool Experiment::isValidEpisode()
{
	return m_episodeIndex > 0 && m_episodeIndex <= m_totalNumEpisodes;
}

void Experiment::nextEpisode()
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

bool Experiment::isFirstEpisode()
{
	return m_episodeIndex == 1;
}

bool Experiment::isLastEpisode()
{
	if (isEvaluationEpisode())
		return m_evalEpisodeIndex == m_numEvaluations*m_numEpisodesPerEvaluation;
	return m_trainingEpisodeIndex == m_numTrainingEpisodes.get();
}

Experiment::~Experiment()
{
	if (m_pProgressTimer)
		delete m_pProgressTimer;
}

Experiment::Experiment(ConfigNode* pConfigNode)
{
	if (!pConfigNode) return;

	m_progUpdateFreq = DOUBLE_PARAM(pConfigNode, "Progress-Update-Freq", "Progress update frequency (seconds)", 1.0);
	m_randomSeed = INT_PARAM(pConfigNode, "Random-Seed", "Random seed used to generate random sequences of numbers", 1);

	m_numTrainingEpisodes = INT_PARAM(pConfigNode, "Num-Episodes", "Number of episodes. Zero if we only want to run one evaluation episode", 1000);
	m_evalFreq = INT_PARAM(pConfigNode, "Eval-Freq", "Evaluation frequency (in episodes). If zero then only training episodes will be run", 10);

	m_episodeLength = DOUBLE_PARAM(pConfigNode, "Episode-Length", "Length of an episode(seconds)", 10.0);

	reset();	//calculate all the variables not given as parameters
				//and reset counters

	m_pProgressTimer = new Timer();

	srand((unsigned int)m_randomSeed.get());
}


void Experiment::timestep(State* s, Action* a, State* s_p, Reward* r)
{
	char msg[1024];


	double time = m_pProgressTimer->getElapsedTime();//(double)(currentCounter - m_lastProgressReportCounter) / (double)m_counterFreq;

	if (time > m_progUpdateFreq.get() || (isLastStep() && isLastEpisode()))
	{
		CrossPlatform::sprintf_s(msg, 1024, "%f", SimionApp::get()->pExperiment->getExperimentProgress()*100.0);
		Logger::logMessage(Progress, msg);
		m_pProgressTimer->start();
	}

	bool evalEpisode = isEvaluationEpisode();
	if (isFirstEpisode() && isFirstStep())
		SimionApp::get()->pLogger->firstEpisode();

	unsigned int episodeIndex = getRelativeEpisodeIndex();
	if (isFirstStep())
		SimionApp::get()->pLogger->firstStep();

	//update stats
	//output step-stats
	SimionApp::get()->pLogger->timestep(s, a, s_p, r);

	if (isLastStep())
		SimionApp::get()->pLogger->lastStep();

	if (isLastEpisode() && (isLastStep()))
		SimionApp::get()->pLogger->lastEpisode();
}