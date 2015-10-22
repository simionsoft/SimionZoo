#include <stdafx.h>
#include "experiment.h"
#include "parameters.h"
#include "states-and-actions.h"
#include "world.h"


CExperiment::~CExperiment()
{
}

CExperiment::CExperiment(CParameters* pParameters)
{
	m_randomSeed= (int) pParameters->getDouble("RANDOM_SEED");
	srand(m_randomSeed);

	m_numEpisodes= (int) pParameters->getDouble("NUM_EPISODES");
	m_numSteps= (int) (pParameters->getDouble("EPISODE_LENGTH")/CWorld::getDT());
	m_step= 0;
	m_episode= 0;

/*
		LOG_OUTPUT_DIR : output-logs
		LOG_FILE_PREFIX : windturbine
		LOG_EVALUATION_EPISODES : 1
		LOG_TRAINING_EPISODES : 1
		LOG_EVALUATION_AVG_REWARDS : 1
		LOG_FREQ : 0.25 //seconds
*/

	strcpy_s(m_outputDir,MAX_FILENAME_LENGTH,pParameters->getStringPtr("LOG_OUTPUT_DIR"));
	strcpy_s(m_filePrefix,MAX_FILENAME_LENGTH,pParameters->getStringPtr("LOG_FILE_PREFIX"));

	m_bLogEvaluationEpisodes= (1.0 == pParameters->getDouble("LOG_EVALUATION_EPISODES"));
	m_bLogTrainingEpisodes= (1.0 == pParameters->getDouble("LOG_TRAINING_EPISODES"));
	m_bLogEvaluationSummary= (1.0 == pParameters->getDouble("LOG_EVALUATION_AVG_REWARDS"));
	m_evalFreq= (unsigned int)pParameters->getDouble("EVAL_FREQ");
	m_logFreq= pParameters->getDouble("LOG_FREQ");

	m_pFile= (void*) 0;

	m_lastLogTime= 0.0;
	m_episodeRewards= 0.0;

	_mkdir(m_outputDir);

	QueryPerformanceFrequency((LARGE_INTEGER*)&m_counterFreq);
	QueryPerformanceCounter((LARGE_INTEGER*)&m_lastCounter);
}

bool CExperiment::isEvaluationEpisode()
{
	if (m_evalFreq==0) return false;
	if (m_episode==1 || (m_episode % m_evalFreq==0))
		return true;
	return false;
}

double CExperiment::getProgress()
{
	double progress= ((double) m_step-1 + (m_episode-1)*m_numSteps)
						/ ((double) m_numSteps*m_numEpisodes-1);
	return progress;
}

double CExperiment::getCurrentAvgReward()
{
	return m_episodeRewards/(double)max(m_step,1);
}

void CExperiment::writeEpisodeLogFileHeader(CState *s, CAction *a)
{
	fprintf((FILE*) m_pFile,"Time");

	for (int i= 0; i<s->getNumVars(); i++)
	{
		fprintf((FILE*) m_pFile,"/%s",s->getName(i));
	}

	for (int i= 0; i<a->getNumVars(); i++)
	{
		fprintf((FILE*) m_pFile,"/%s",a->getName(i));
	}
	fprintf((FILE*) m_pFile,"\n");
}

void CExperiment::writeEpisodeStep(CState *s, CAction *a, CState *s_p, double r)
{
	if (!m_pFile) return;

	fprintf((FILE*) m_pFile,"%.3f ",CWorld::getStepStartT());
	for (int i= 0; i<s->getNumVars(); i++)
	{
		fprintf((FILE*) m_pFile,"%.3f ",s->getValue(i));
	}
	for (int i= 0; i<a->getNumVars(); i++)
	{
		fprintf((FILE*) m_pFile,"%.3f ",a->getValue(i));
	}
	fprintf((FILE*) m_pFile,"%.3f\n",r);
}

bool CExperiment::isCurrentEpisodeLogged()
{
	bool bEval= isEvaluationEpisode();
	return (m_bLogEvaluationEpisodes && bEval) || (m_bLogTrainingEpisodes && !bEval);
}

void CExperiment::logStep(CState *s, CAction *a, CState *s_p, double r)
{
	bool bLog= isCurrentEpisodeLogged();

	if (m_step==1) m_episodeRewards= r;
	else m_episodeRewards+= r;


	//FIRST STEP IN EPISODE????
	if (bLog && m_step==1)
	{
		m_lastLogTime= 0.0;
		openEpisodeLogFile();
		if (m_pFile)
			writeEpisodeLogFileHeader(s,a);
	}

	if (bLog && (CWorld::getStepStartT()-m_lastLogTime>=m_logFreq || m_step==1))
	{
		writeEpisodeStep(s,a,s_p,r);
		m_lastLogTime= CWorld::getStepStartT();
	}

	//LAST STEP IN EPISODE????
	if (m_step==m_numSteps)
	{
		//close current log file
		if (bLog && m_pFile)
		{
			fclose((FILE*) m_pFile);
			m_pFile= 0;
		}

		//SAVE AVERAGE REWARDS IN SUMMARY FILE????
		if (m_bLogEvaluationSummary && isEvaluationEpisode())
		{
			writeEpisodeSummary();
		}
	}
	

	//print progress
	__int64 currentCounter;
	QueryPerformanceCounter((LARGE_INTEGER*)&currentCounter);
	double time= (double)(currentCounter-m_lastCounter) / (double) m_counterFreq;
	if (time>0.5) //each .5 secs?
	{
		printf("EPISODE: %d/%d STEP %d/%d (%.2f%%) Avg.Reward: %.2f        \r"
			,m_episode,m_numEpisodes,m_step,m_numSteps,getProgress()*100.0,getCurrentAvgReward());
		m_lastCounter= currentCounter;
	}

}

void CExperiment::openEpisodeLogFile()
{
	char filename[1024];
	if (!isEvaluationEpisode())
		sprintf_s(filename,1024,"%s/%s-training-%d-%d.txt",m_outputDir,m_filePrefix
		,m_randomSeed,m_episode);
	else
		sprintf_s(filename,1024,"%s/%s-evaluation-%d-%d.txt",m_outputDir,m_filePrefix
		,m_randomSeed,m_episode);


	fopen_s((FILE**) &m_pFile,filename,"w");
}

void CExperiment::writeEpisodeSummary()
{
	FILE* pFile;
	char filename[1024];
	sprintf_s(filename,1024,"%s/%s-summary-%d.txt",m_outputDir,m_filePrefix,m_randomSeed);
	
	if (m_episode==1)
		fopen_s(&pFile,filename,"w");
	else fopen_s(&pFile,filename,"a");
	if (pFile)
	{
		fprintf(pFile,"%d %f\n",m_episode,getCurrentAvgReward());
		fclose(pFile);
	}
}