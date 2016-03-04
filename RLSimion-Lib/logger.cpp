#include "stdafx.h"
#include "logger.h"
#include "world.h"
#include "named-var-set.h"
#include "globals.h"
#include "experiment.h"
#include "parameters.h"
#include "stats.h"

#define MAX_FILENAME_LENGTH 1024

CLogger::CLogger(CParameters* pParameters)
{
	m_outputDir = new char[MAX_FILENAME_LENGTH];
	m_filePrefix = new char[MAX_FILENAME_LENGTH];

	if (pParameters)
	{
		sprintf_s(m_outputDir, MAX_FILENAME_LENGTH, "../logs/%s", pParameters->getConstString( "Ouput-directory"));
		strcpy_s(m_filePrefix, MAX_FILENAME_LENGTH, pParameters->getConstString("Prefix"));

		m_bLogEvaluationEpisodes = pParameters->getConstBoolean("Log-eval-episodes", false);
		m_bLogTrainingEpisodes = pParameters->getConstBoolean("Log-training-episodes", false);
		m_bLogEvaluationExperiment = pParameters->getConstBoolean("Log-eval-experiment", true);
		m_bLogTrainingExperiment = pParameters->getConstBoolean( "Log-training-experiment", false);

		m_logFreq = pParameters->getConstDouble( "Log-Freq", 0.25);
		m_progUpdateFreq = pParameters->getConstDouble( "Progress-Update-Freq", 0.5);

		_mkdir(m_outputDir);
	}
	else
	{
		//default values for safety
		m_outputDir[0] = 0;
		m_filePrefix[0] = 0;
		m_bLogEvaluationEpisodes = false;
		m_bLogEvaluationExperiment = false;
		m_bLogTrainingEpisodes = true;
		m_bLogTrainingExperiment = false;
		m_logFreq = 0.0;
		m_progUpdateFreq = 0.5;
	}

	QueryPerformanceFrequency((LARGE_INTEGER*)&m_counterFreq);
	m_episodeStartCounter = 0;
	m_experimentStartCounter = 0;
	m_lastProgressReportCounter = 0;
	m_lastLogSimulationT = 0.0;
}


CLogger::~CLogger()
{
	delete [] m_outputDir;
	delete [] m_filePrefix;
}

void CLogger::getLogFilename(char* buffer, int bufferSize, bool episodeLog, bool evaluation)
{
	if (episodeLog)
	{
		if (!evaluation)
			sprintf_s(buffer, bufferSize, "%s/%s-train-epis-%d.txt", m_outputDir, m_filePrefix
			, RLSimion::g_pExperiment->m_expProgress.getEvaluationEpisodeIndex());
		else
			sprintf_s(buffer, bufferSize, "%s/%s-eval-epis-%d.txt", m_outputDir, m_filePrefix
			, RLSimion::g_pExperiment->m_expProgress.getTrainingEpisodeIndex());
	}
	else
	{
		if (!evaluation)
			sprintf_s(buffer, bufferSize, "%s/%s-train-exp.txt", m_outputDir, m_filePrefix);
		else
			sprintf_s(buffer, bufferSize, "%s/%s-eval-exp.txt", m_outputDir, m_filePrefix);
	}
}

FILE* CLogger::openLogFile(bool create, bool episodeLog, bool evalEpisode)
{
	FILE* pFile;
	char filename[1024];

	getLogFilename(filename, 1024, episodeLog, evalEpisode);

	if (create)
	{
		fopen_s(&pFile, filename, "w");
		if (pFile)
		{
			writeLogFileHeader(pFile, episodeLog, evalEpisode);
			fclose(pFile);
		}
		return 0;
	}
	else
	{
		fopen_s(&pFile, filename, "a");
		return pFile;
	}
}

void CLogger::writeLogFileHeader(FILE* pFile, bool episodeLog, bool evaluationLog)
{
	if (!pFile) return;
	if (episodeLog)
		fprintf_s(pFile, "Time ");
	else
		fprintf_s(pFile, "Episode Experiment-Time Episode-Duration ");

	for (auto it = m_stats.begin(); it != m_stats.end(); it++)
	{
		fprintf_s(pFile, "%s\\%s ", (*it)->getKey(), (*it)->getSubkey());
	}
	fprintf_s(pFile, "\n");
}

void CLogger::writeExperimentLogData(bool evalEpisode)
{
	if (logCurrentEpisodeInExperiment(evalEpisode))
	{
		FILE* pFile = openLogFile(false, false, evalEpisode);

		//output the episode index, the elapsed time since the experiment started and the last episode's time length
		__int64 currentCounter;
		QueryPerformanceCounter((LARGE_INTEGER*)&currentCounter);
		double experimentTime = (double)(currentCounter - m_experimentStartCounter) / (double)m_counterFreq;
		double episodeDuration = (double)(currentCounter - m_episodeStartCounter) / (double)m_counterFreq;
		int episodeIndex;
		if (RLSimion::g_pExperiment->m_expProgress.isEvaluationEpisode())
			episodeIndex = RLSimion::g_pExperiment->m_expProgress.getEvaluationEpisodeIndex();
		else
			episodeIndex = RLSimion::g_pExperiment->m_expProgress.getTrainingEpisodeIndex();
		fprintf(pFile, "%d %.3f %.3f ", episodeIndex, experimentTime, episodeDuration);
		//output the stats
		for (auto it = m_stats.begin(); it != m_stats.end(); it++)
		{
			fprintf(pFile, "%.3f(%.3f) ", (*it)->getStatsInfo()->getAvg(), (*it)->getStatsInfo()->getStdDev());
		}
		fprintf(pFile, "\n");
		fclose(pFile);
	}
}

void CLogger::writeEpisodeLogData(bool evalEpisode)
{
	if (logCurrentEpisode(evalEpisode))
	{
		FILE* pFile = openLogFile(false, true, evalEpisode);

		fprintf(pFile, "%.3f ", RLSimion::g_pWorld->getT());

		for (auto it = m_stats.begin(); it != m_stats.end(); it++)
		{
			fprintf(pFile, "%.3f ", (*it)->getValue());
		}
		fprintf(pFile, "\n");
		fclose(pFile);
	}
}

bool CLogger::logCurrentEpisode(bool evalEpisode)
{
	return (evalEpisode && m_bLogEvaluationEpisodes) || (!evalEpisode && m_bLogTrainingEpisodes);
}

bool CLogger::logCurrentEpisodeInExperiment(bool evalEpisode)
{
	return (evalEpisode && m_bLogEvaluationExperiment) || (!evalEpisode && m_bLogTrainingExperiment);
}

void CLogger::firstEpisode(bool evalEpisode)
{
	//set episode start time
	QueryPerformanceCounter((LARGE_INTEGER*)&m_experimentStartCounter);

	if (m_bLogEvaluationExperiment) openLogFile(true, false, evalEpisode);
}

void CLogger::firstStep(bool evalEpisode)
{
	//set episode start time
	QueryPerformanceCounter((LARGE_INTEGER*)&m_episodeStartCounter);
	m_lastLogSimulationT = 0.0;
	//reset stats
	for (auto it = m_stats.begin(); it != m_stats.end(); it++)
		(*it)->reset();


	//create the episode log file
	if (logCurrentEpisode(evalEpisode))
		openLogFile(true, true, evalEpisode);
}

void CLogger::timestep(bool evalEpisode)
{
	bool bLog = logCurrentEpisode(evalEpisode);

	//update experiment stats
	for (auto iterator = m_stats.begin(); iterator != m_stats.end(); iterator++)
	{
		(*iterator)->addExperimentSample();
	}

	//output episode log data
	if (bLog && (RLSimion::g_pWorld->getStepStartT() - m_lastLogSimulationT >= m_logFreq
		|| RLSimion::g_pExperiment->m_expProgress.isFirstStep()))
	{
		writeEpisodeLogData(evalEpisode);
		m_lastLogSimulationT = RLSimion::g_pWorld->getStepStartT();
	}


	//print progress
	__int64 currentCounter;
	QueryPerformanceCounter((LARGE_INTEGER*)&currentCounter);
	double time = (double)(currentCounter - m_lastProgressReportCounter) / (double)m_counterFreq;

	if (time>m_progUpdateFreq)
	{
		CLogger::logMessage(Progress, RLSimion::g_pExperiment->m_expProgress.getProgressString());
		m_lastProgressReportCounter = currentCounter;
	}

}

void CLogger::lastStep(bool evalEpisode)
{
	writeExperimentLogData(evalEpisode);
}

void CLogger::lastEpisode(bool evalEpisode)
{
	//so far, we are doing nothing
}


void CLogger::addVarToStats(const char* key, const char* subkey, double* variable)
{
	//all stats added by the loaded classes are calculated
	//if (m_pParameters->getChild(key))
	m_stats.push_back(new CStats(key, subkey, (void*) variable, Double));
}

void CLogger::addVarToStats(const char* key, const char* subkey, int* variable)
{
	//all stats added by the loaded classes are calculated
	//if (m_pParameters->getChild(key))
	m_stats.push_back(new CStats(key, subkey, (void*) variable, Int));
}

void CLogger::addVarToStats(const char* key, const char* subkey, unsigned int* variable)
{
	//all stats added by the loaded classes are calculated
	//if (m_pParameters->getChild(key))
	m_stats.push_back(new CStats(key, subkey, (void*)variable, UnsignedInt));
}

void CLogger::addVarSetToStats(const char* key, CNamedVarSet* varset)
{
	for (int i = 0; i < varset->getNumVars(); i++)
	{
		m_stats.push_back(new CStats(key, varset->getName(i), varset->getValuePtr(i), Double));
	}
}

void CLogger::logMessage(MessageType type, const char* message)
{
	char c;
	switch (type)
	{
	case Warning:
		printf("WARNING: %s\n", message);
		break;
	case Progress:
		//extra spaces to avoid overwriting only partially previous message
		printf("PROGRESS: %s                     \r",message);
		break;
	case Info:
		printf("INFO: %s", message);
		break;
	case Error:
		printf("FATAL ERROR: %s\n");
		scanf_s("%c", &c);
		exit(-1);
	}
}