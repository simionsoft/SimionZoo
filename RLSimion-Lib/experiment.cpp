#include "stdafx.h"
#include "experiment.h"
#include "parameters.h"
#include "parameter.h"
#include "states-and-actions.h"
#include "world.h"

double CExperimentProgress::getExperimentProgress()
{
	double progress = ((double)m_step - 1 + (m_episode - 1)*m_numSteps)
		/ ((double)m_numSteps*m_numEpisodes - 1);
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
	m_randomSeed= (int) pParameters->getParameter("RANDOM_SEED")->getDouble();
	srand(m_randomSeed);

	m_expProgress.setNumEpisodes( (unsigned int)pParameters->getParameter("NUM_EPISODES")->getDouble());
	m_expProgress.setNumSteps( (unsigned int)(pParameters->getParameter("EPISODE_LENGTH")->getDouble() / CWorld::getDT()));

	m_evalFreq = (unsigned int)pParameters->getParameter("EVAL_FREQ")->getDouble();

/*
		LOG_OUTPUT_DIR : output-logs
		LOG_FILE_PREFIX : windturbine
		LOG_EVALUATION_EPISODES : 1
		LOG_TRAINING_EPISODES : 1
		LOG_EVALUATION_AVG_REWARDS : 1
		LOG_FREQ : 0.25 //seconds
*/
	CParameters* pLogParameters = pParameters->getChild("LOG");

	strcpy_s(m_outputDir, MAX_FILENAME_LENGTH, pLogParameters->getParameter("OUTPUT_DIR")->getStringPtr());
	strcpy_s(m_filePrefix, MAX_FILENAME_LENGTH, pLogParameters->getParameter("FILE_PREFIX")->getStringPtr());

	m_bLogEvaluationEpisodes = (1.0 == pLogParameters->getParameter("EVALUATION_EPISODES")->getDouble());
	m_bLogTrainingEpisodes = (1.0 == pLogParameters->getParameter("TRAINING_EPISODES")->getDouble());
	m_bLogEvaluationSummary = (1.0 == pLogParameters->getParameter("EVALUATION_AVG_REWARDS")->getDouble());
	m_logFreq = pLogParameters->getParameter("FREQ")->getDouble();

	m_pFile= (void*) 0;

	m_lastLogTime= 0.0;
	m_episodeRewards= 0.0;
	m_lastEvaluationAvgReward = 0.0;

	_mkdir(m_outputDir);

	QueryPerformanceFrequency((LARGE_INTEGER*)&m_counterFreq);
	QueryPerformanceCounter((LARGE_INTEGER*)&m_lastCounter);
}

bool CExperiment::isEvaluationEpisode()
{
	if (m_evalFreq==0) return false;
	if (m_expProgress.getEpisode() == 1 || (m_expProgress.getEpisode() % m_evalFreq == 0))
		return true;
	return false;
}



double CExperiment::getCurrentAvgReward()
{
	return m_lastEvaluationAvgReward;// m_episodeRewards / (double)max(m_step, 1);
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
	
	//FIRST STEP IN EPISODE????
	if ( m_expProgress.isFirstStep() )
	{
		m_episodeRewards = r;
		m_lastLogTime = 0.0;

		if (bLog)
		{
			openEpisodeLogFile();
			if (m_pFile)
				writeEpisodeLogFileHeader(s, a);
		}
	}
	else m_episodeRewards+= r;




	if (bLog && (CWorld::getStepStartT() - m_lastLogTime >= m_logFreq || m_expProgress.isFirstStep()))
	{
		writeEpisodeStep(s,a,s_p,r);
		m_lastLogTime= CWorld::getStepStartT();
	}

	//LAST STEP IN EPISODE????
	if (m_expProgress.isLastStep())
	{
		//close current log file
		if (bLog && m_pFile)
		{
			fclose((FILE*) m_pFile);
			m_pFile= 0;
		}
		if (isEvaluationEpisode())
		{
			m_lastEvaluationAvgReward= m_episodeRewards / (double)std::max(m_expProgress.getStep(), (unsigned int)1);
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
	double time= (double)(currentCounter-m_lastCounter) / (double) m_counterFreq;
	if (time>0.5) //each .5 secs?
	{
		printf("EPISODE: %d/%d STEP %d/%d (%.2f%%) Avg.Reward: %.4f        \r"
			,m_expProgress.getEpisode(),m_expProgress.getNumEpisodes()
			,m_expProgress.getStep(),m_expProgress.getNumSteps(),m_expProgress.getExperimentProgress()*100.0
			,getCurrentAvgReward());
		m_lastCounter= currentCounter;
	}

}

void CExperiment::openEpisodeLogFile()
{
	char filename[1024];
	if (!isEvaluationEpisode())
		sprintf_s(filename,1024,"%s/%s-training-%d-%d.txt",m_outputDir,m_filePrefix
		,m_randomSeed,m_expProgress.getEpisode());
	else
		sprintf_s(filename,1024,"%s/%s-evaluation-%d-%d.txt",m_outputDir,m_filePrefix
		, m_randomSeed, m_expProgress.getEpisode());


	fopen_s((FILE**) &m_pFile,filename,"w");
}

void CExperiment::writeEpisodeSummary()
{
	FILE* pFile;
	char filename[1024];
	sprintf_s(filename,1024,"%s/%s-summary-%d.txt",m_outputDir,m_filePrefix,m_randomSeed);
	
	if (m_expProgress.isFirstEpisode())
		fopen_s(&pFile,filename,"w");
	else fopen_s(&pFile,filename,"a");
	if (pFile)
	{
		fprintf(pFile,"%d %f\n",m_expProgress.getEpisode(),getCurrentAvgReward());
		fclose(pFile);
	}
}