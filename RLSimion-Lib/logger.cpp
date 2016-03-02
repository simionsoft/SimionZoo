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
		strcpy_s(m_filePrefix, MAX_FILENAME_LENGTH, XMLParameters::getConstString(pParameters->FirstChildElement("Prefix")));

		m_bLogEvaluationEpisodes = XMLParameters::getConstBoolean(pParameters->FirstChildElement("Log-eval-episodes"));
		m_bLogTrainingEpisodes = XMLParameters::getConstBoolean(pParameters->FirstChildElement("Log-training-episodes"));
		m_bLogEvaluationSummary = XMLParameters::getConstBoolean(pParameters->FirstChildElement("Log-eval-avg-rewards"));
		m_logFreq = XMLParameters::getConstDouble(pParameters->FirstChildElement("Freq"));
	}
	else
	{
		//default values for safety
		m_outputDir[0] = 0;
		m_filePrefix[0] = 0;
		m_bLogEvaluationEpisodes = false;
		m_bLogEvaluationSummary = false;
		m_bLogTrainingEpisodes = false;
		m_logFreq = 0.0;
	}

	m_pFile = (void*)0;
	m_lastLogTime = 0.0;
	m_episodeRewards = 0.0;
	m_lastEvaluationAvgReward = 0.0;

	_mkdir(m_outputDir);

	QueryPerformanceFrequency((LARGE_INTEGER*)&m_counterFreq);
	QueryPerformanceCounter((LARGE_INTEGER*)&m_lastCounter);
}


CLogger::~CLogger()
{
	delete [] m_outputDir;
	delete [] m_filePrefix;
}




void CLogger::writeEpisodeLogFileHeader(CState *s, CAction *a, CReward* pReward)
{
	fprintf((FILE*)m_pFile, "Time");

	for (int i = 0; i<s->getNumVars(); i++)
	{
		fprintf((FILE*)m_pFile, "/%s", s->getName(i));
	}

	for (int i = 0; i<a->getNumVars(); i++)
	{
		fprintf((FILE*)m_pFile, "/%s", a->getName(i));
	}

	for (int i = 0; i < pReward->getNumVars(); i++)
	{
		fprintf((FILE*)m_pFile, "/%s", pReward->getName(i));
	}
	fprintf((FILE*)m_pFile, "/r\n");
}

void CLogger::writeEpisodeStep(CState *s, CAction *a, CState *s_p, CReward* pReward)
{
	if (!m_pFile) return;

	fprintf((FILE*)m_pFile, "%.3f ", RLSimion::g_pWorld->getStepStartT());
	for (int i = 0; i<s->getNumVars(); i++)
	{
		fprintf((FILE*)m_pFile, "%.3f ", s->getValue(i));
	}
	for (int i = 0; i<a->getNumVars(); i++)
	{
		fprintf((FILE*)m_pFile, "%.3f ", a->getValue(i));
	}
	for (int i = 0; i < pReward->getNumVars(); i++)
	{
		fprintf((FILE*)m_pFile, "%.3f ", pReward->getValue(i));
	}
	fprintf((FILE*)m_pFile, "%.3f\n", pReward->getSumValue());
}

bool CLogger::isCurrentEpisodeLogged()
{
	bool bEval = RLSimion::g_pExperiment->isEvaluationEpisode();
	return (m_bLogEvaluationEpisodes && bEval) || (m_bLogTrainingEpisodes && !bEval);
}

void CLogger::timestep(CState *s, CAction *a, CState *s_p, CReward* pReward)
{
	//update stats
	for (auto iterator = m_stats.begin(); iterator != m_stats.end(); iterator++)
	{
		(*iterator)->addSample();
	}


	bool bLog = isCurrentEpisodeLogged();
	double r = pReward->getSumValue();

	//FIRST STEP IN EPISODE????
	if (RLSimion::g_pExperiment->m_expProgress.isFirstStep())
	{
		m_episodeRewards = r;
		m_lastLogTime = 0.0;

		if (bLog)
		{
			openEpisodeLogFile();
			if (m_pFile)
				writeEpisodeLogFileHeader(s, a, pReward);
		}
	}
	else m_episodeRewards += r;




	if (bLog && (RLSimion::g_pWorld->getStepStartT() - m_lastLogTime >= m_logFreq || RLSimion::g_pExperiment->m_expProgress.isFirstStep()))
	{
		writeEpisodeStep(s, a, s_p, pReward);
		m_lastLogTime = RLSimion::g_pWorld->getStepStartT();
	}

	//LAST STEP IN EPISODE????
	if (RLSimion::g_pExperiment->m_expProgress.isLastStep())
	{
		//close current log file
		if (bLog && m_pFile)
		{
			fclose((FILE*)m_pFile);
			m_pFile = 0;
		}
		if (RLSimion::g_pExperiment->isEvaluationEpisode())
		{
			m_lastEvaluationAvgReward = m_episodeRewards / (double)std::max(RLSimion::g_pExperiment->m_expProgress.getStep(), (unsigned int)1);
			//SAVE AVERAGE REWARDS IN SUMMARY FILE????
			if (m_bLogEvaluationSummary)
			{
				writeEpisodeSummary();
			}
		}
	}


	//print progress
	__int64 currentCounter;
	QueryPerformanceCounter((LARGE_INTEGER*)&currentCounter);
	double time = (double)(currentCounter - m_lastCounter) / (double)m_counterFreq;
	if (time>0.5) //each .5 secs?
	{
		printf("EPISODE: %d/%d STEP %d/%d (%.2f%%) Avg.Reward: %.4f        \r"
			, RLSimion::g_pExperiment->m_expProgress.getEpisode(), RLSimion::g_pExperiment->m_expProgress.getNumEpisodes()
			, RLSimion::g_pExperiment->m_expProgress.getStep(), RLSimion::g_pExperiment->m_expProgress.getNumSteps()
			, RLSimion::g_pExperiment->m_expProgress.getExperimentProgress()*100.0
			, m_lastEvaluationAvgReward);
		m_lastCounter = currentCounter;
	}

}

void CLogger::openEpisodeLogFile()
{
	char filename[1024];
	if (!RLSimion::g_pExperiment->isEvaluationEpisode())
		sprintf_s(filename, 1024, "%s/%s-training-%d.txt", m_outputDir, m_filePrefix
		, RLSimion::g_pExperiment->m_expProgress.getEpisode());
	else
		sprintf_s(filename, 1024, "%s/%s-evaluation-%d.txt", m_outputDir, m_filePrefix
		, RLSimion::g_pExperiment->m_expProgress.getEpisode());


	fopen_s((FILE**)&m_pFile, filename, "w");
}

void CLogger::writeEpisodeSummary()
{
	FILE* pFile;
	char filename[1024];
	sprintf_s(filename, 1024, "%s/%s-summary.txt", m_outputDir, m_filePrefix);

	if (RLSimion::g_pExperiment->m_expProgress.isFirstEpisode())
		fopen_s(&pFile, filename, "w");
	else fopen_s(&pFile, filename, "a");
	if (pFile)
	{
		fprintf(pFile, "%d %f\n", RLSimion::g_pExperiment->m_expProgress.getEpisode()
			, m_lastEvaluationAvgReward);
		fclose(pFile);
	}
}


void CLogger::addVarToStats(Stat key, const char* subkey, double* variable)
{
	//all stats added by the loaded classes are calculated
	//if (m_pParameters->FirstChildElement(key))
	m_stats.push_back(new CStat(key, subkey, variable));
}

void CLogger::addVarSetToStats(Stat key, CNamedVarSet* varset)
{
	for (int i = 0; i < varset->getNumVars(); i++)
	{
		m_stats.push_back(new CStat(key, varset->getName(i), varset->getValuePtr(i)));
	}
}