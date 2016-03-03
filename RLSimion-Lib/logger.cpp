#include "stdafx.h"
#include "logger.h"
#include "world.h"
#include "named-var-set.h"
#include "globals.h"
#include "experiment.h"
#include "xml-parameters.h"
#include "stats.h"

#define MAX_FILENAME_LENGTH 1024

CLogger::CLogger(tinyxml2::XMLElement* pParameters)
{
	m_outputDir = new char[MAX_FILENAME_LENGTH];
	m_filePrefix = new char[MAX_FILENAME_LENGTH];

	if (pParameters)
	{
		sprintf_s(m_outputDir, MAX_FILENAME_LENGTH, "../logs/%s"
			, pParameters->FirstChildElement("Ouput-directory")->GetText());
		strcpy_s(m_filePrefix, MAX_FILENAME_LENGTH, XMLUtils::getConstString(pParameters->FirstChildElement("Prefix")));

		if (pParameters->FirstChildElement("Log-eval-episodes"))
			m_bLogEvaluationEpisodes = XMLUtils::getConstBoolean(pParameters->FirstChildElement("Log-eval-episodes"));
		else m_bLogEvaluationEpisodes = false;
		if (pParameters->FirstChildElement("Log-training-episodes"))
			m_bLogTrainingEpisodes = XMLUtils::getConstBoolean(pParameters->FirstChildElement("Log-training-episodes"));
		else m_bLogTrainingEpisodes = false;
		if (pParameters->FirstChildElement("Log-eval-experiment"))
			m_bLogEvaluationExperiment = XMLUtils::getConstBoolean(pParameters->FirstChildElement("Log-eval-experiment"));
		else m_bLogEvaluationExperiment = false;

		m_logFreq = XMLUtils::getConstDouble(pParameters->FirstChildElement("Log-Freq"));
		if (pParameters->FirstChildElement("Progress-Update-Freq"))
			m_progUpdateFreq = XMLUtils::getConstDouble(pParameters->FirstChildElement("Progress-Update-Freq"));
		else m_progUpdateFreq = 0.5;

		_mkdir(m_outputDir);
	}
	else
	{
		//default values for safety
		m_outputDir[0] = 0;
		m_filePrefix[0] = 0;
		m_bLogEvaluationEpisodes = false;
		m_bLogEvaluationExperiment = false;
		m_bLogTrainingEpisodes = false;
		m_logFreq = 0.0;
		m_progUpdateFreq = 0.5;
	}

	QueryPerformanceFrequency((LARGE_INTEGER*)&m_counterFreq);
	m_episodeStartT = 0;
	m_experimentStartT = 0;
	m_lastLogSimulationT = 0.0;
	m_lastProgressReportT = 0;
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
			, RLSimion::g_pExperiment->m_expProgress.getEpisode());
		else
			sprintf_s(buffer, bufferSize, "%s/%s-eval-epis-%d.txt", m_outputDir, m_filePrefix
			, RLSimion::g_pExperiment->m_expProgress.getEpisode());
	}
	else
	{
		if (!evaluation)
			sprintf_s(buffer, bufferSize, "%s/%s-train-exp-%d.txt", m_outputDir, m_filePrefix
			, RLSimion::g_pExperiment->m_expProgress.getEpisode());
		else
			sprintf_s(buffer, bufferSize, "%s/%s-eval-exp-%d.txt", m_outputDir, m_filePrefix
			, RLSimion::g_pExperiment->m_expProgress.getEpisode());
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

	for (auto it = m_stats.begin(); it != m_stats.end(); it++)
	{
		fprintf_s(pFile, "%s/%s ", (*it)->getKey(), (*it)->getSubkey());
	}
	fprintf_s(pFile, "\n");
}

void CLogger::writeExperimentLogData(bool evalEpisode)
{
	if (evalEpisode && m_bLogEvaluationExperiment)
	{
		FILE* pFile = openLogFile(false, false, evalEpisode);

		for (auto it = m_stats.begin(); it != m_stats.end(); it++)
		{
			fprintf(pFile, "%.3f ", (*it)->getValue());
		}
		fprintf(pFile, "\n");
		fclose(pFile);
	}
}

void CLogger::writeEpisodeLogData(bool evalEpisode)
{
	if ((evalEpisode && m_bLogEvaluationEpisodes) || (evalEpisode && m_bLogTrainingEpisodes))
	{
		FILE* pFile = openLogFile(false, true, evalEpisode);

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

void CLogger::firstEpisode(bool evalEpisode)
{
	//set episode start time
	QueryPerformanceCounter((LARGE_INTEGER*)&m_experimentStartT);

	if (m_bLogEvaluationExperiment) openLogFile(true, false, evalEpisode);
}

void CLogger::firstStep(bool evalEpisode)
{
	//set episode start time
	QueryPerformanceCounter((LARGE_INTEGER*)&m_episodeStartT);
	m_lastLogSimulationT = 0.0;
	//reset stats
	for (auto iterator = m_stats.begin(); iterator != m_stats.end(); iterator++)
	{
		(*iterator)->reset();
	}

	//create the episode log file
	if (logCurrentEpisode(evalEpisode))
		openLogFile(true, true, evalEpisode);
}

void CLogger::timestep(bool evalEpisode)
{
	bool bLog = logCurrentEpisode(evalEpisode);

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
	double time = (double)(currentCounter - m_lastProgressReportT) / (double)m_counterFreq;
	if (time>0.5) //each .5 secs?
	{
		printf("EPISODE: %d/%d STEP %d/%d (%.2f%%)\r"
			, RLSimion::g_pExperiment->m_expProgress.getEpisode(), RLSimion::g_pExperiment->m_expProgress.getNumEpisodes()
			, RLSimion::g_pExperiment->m_expProgress.getStep(), RLSimion::g_pExperiment->m_expProgress.getNumSteps()
			, RLSimion::g_pExperiment->m_expProgress.getExperimentProgress()*100.0);
		m_lastProgressReportT = currentCounter;
	}

}

void CLogger::lastStep(bool evalEpisode)
{
	//update experiment stats
	for (auto iterator = m_stats.begin(); iterator != m_stats.end(); iterator++)
	{
		(*iterator)->addExperimentSample();
	}

	writeExperimentLogData(evalEpisode);
}

void CLogger::lastEpisode(bool evalEpisode)
{
	//so far, we are doing nothing
}


void CLogger::addVarToStats(const char* key, const char* subkey, double* variable)
{
	//all stats added by the loaded classes are calculated
	//if (m_pParameters->FirstChildElement(key))
	m_stats.push_back(new CStats(key, subkey, variable));
}

void CLogger::addVarSetToStats(const char* key, CNamedVarSet* varset)
{
	for (int i = 0; i < varset->getNumVars(); i++)
	{
		m_stats.push_back(new CStats(key, varset->getName(i), varset->getValuePtr(i)));
	}
}