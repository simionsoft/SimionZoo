#include "stdafx.h"
#include "experiment.h"
#include "states-and-actions.h"
#include "world.h"
#include "reward.h"
#include "parameters-xml-helper.h"

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

CExperiment::CExperiment(tinyxml2::XMLElement* pParameters)
{
	m_randomSeed= XMLParameters::getConstInteger(pParameters->FirstChildElement("Random-Seed"));
	srand(m_randomSeed);

	m_expProgress.setNumEpisodes(XMLParameters::getConstInteger(pParameters->FirstChildElement("Num-Episodes")));
	m_expProgress.setNumSteps((unsigned int)
		(XMLParameters::getConstDouble(pParameters->FirstChildElement("Episode-Length")) / CWorld::getDT()));

	m_evalFreq = XMLParameters::getConstInteger(pParameters->FirstChildElement("Eval-Freq"));

	tinyxml2::XMLElement* pLogParameters = pParameters->FirstChildElement("LOG");

	strcpy_s(m_outputDir, MAX_FILENAME_LENGTH, pLogParameters->FirstChildElement("Ouput-directory")->Value());
	strcpy_s(m_filePrefix, MAX_FILENAME_LENGTH, pLogParameters->FirstChildElement("Prefix")->Value());

	m_bLogEvaluationEpisodes = XMLParameters::getBoolean(pLogParameters->FirstChildElement("Log-eval-episodes"));
	m_bLogTrainingEpisodes = XMLParameters::getBoolean(pLogParameters->FirstChildElement("Log-training-episodes"));
	m_bLogEvaluationSummary = XMLParameters::getBoolean(pLogParameters->FirstChildElement("Log-eval-avg-rewards"));
	m_logFreq = XMLParameters::getConstDouble(pLogParameters->FirstChildElement("Freq"));

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

void CExperiment::writeEpisodeLogFileHeader(CState *s, CAction *a, CReward* pReward)
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

	for (int i = 0; i < pReward->getNumRewardComponents(); i++)
	{
		fprintf((FILE*)m_pFile, "/r_%d", i);
	}
	fprintf((FILE*) m_pFile,"/r\n");
}

void CExperiment::writeEpisodeStep(CState *s, CAction *a, CState *s_p, CReward* pReward)
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
	for (int i = 0; i < pReward->getNumRewardComponents(); i++)
	{
		fprintf((FILE*)m_pFile, "%.3f ", pReward->getLastRewardComponent(i));
	}
	fprintf((FILE*) m_pFile,"%.3f\n",pReward->getLastScalarReward());
}

bool CExperiment::isCurrentEpisodeLogged()
{
	bool bEval= isEvaluationEpisode();
	return (m_bLogEvaluationEpisodes && bEval) || (m_bLogTrainingEpisodes && !bEval);
}

void CExperiment::logStep(CState *s, CAction *a, CState *s_p, CReward* pReward)
{
	bool bLog= isCurrentEpisodeLogged();
	double r = pReward->getLastScalarReward();
	
	//FIRST STEP IN EPISODE????
	if ( m_expProgress.isFirstStep() )
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
	else m_episodeRewards+= r;




	if (bLog && (CWorld::getStepStartT() - m_lastLogTime >= m_logFreq || m_expProgress.isFirstStep()))
	{
		writeEpisodeStep(s,a,s_p,pReward);
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