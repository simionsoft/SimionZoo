#include "stdafx.h"
#include "parameterscheduler.h"
#include "parameters.h"
#include "globals.h"
#include "experiment.h"

CParameterScheduler::CParameterScheduler(char *configFile,CParameters* pParameters)
{
	char configLine[MAX_STRING_SIZE], *name;
	char decay[MAX_STRING_SIZE];

	m_numParameterSchedules = 0;
	m_pScheduleData = 0;
	m_pParameters = pParameters;		// don't free the memory!!!

	if (!configFile) return;

	CParameters* pSchedules = new CParameters(configFile);
	
	int numParameterSchedules = pSchedules->getNumParameters();

	m_pScheduleData= new CScheduleData[numParameterSchedules];

	for (int i= 0; i<numParameterSchedules; i++)
	{
		//get the parameter's name from the schedule file
		name = pSchedules->getParameterName(i);

		m_pScheduleData[i].pName = new char[strlen(name) + 1];
		strcpy_s(m_pScheduleData[i].pName, strlen(name) + 1, name);

		//parse the schedule line
		strcpy_s(configLine, MAX_STRING_SIZE, pSchedules->getStringPtr(i));
		if (sscanf_s(configLine,"%lf,%lf,%lf,%s",&m_pScheduleData[i].start
			,&m_pScheduleData[i].end, &m_pScheduleData[i].evalValue,decay,MAX_STRING_SIZE)==4)
		{
			if (strcmp(decay,"linear")==0)
				m_pScheduleData[i].decayType= LINEAR_DECAY;
			else if (strcmp(decay,"quadratic")==0)
				m_pScheduleData[i].decayType= QUADRATIC_DECAY;
			else assert(0);
			m_numParameterSchedules++;
		}
		else
		{
			printf("Error reading parameter schedule in:\nFILE= %s\nLINE %d=%s\n"
				,configFile,i,configLine);
		}
	}
	assert(m_numParameterSchedules==numParameterSchedules);

	delete pSchedules;
}


CParameterScheduler::~CParameterScheduler()
{
	if (m_pScheduleData) delete [] m_pScheduleData;
}



void CParameterScheduler::update()
{
	double progress;
	bool eval= g_pExperiment->isEvaluationEpisode();

	if (!eval) 
		progress= g_pExperiment->getProgress();

	for (int i= 0; i<m_numParameterSchedules; i++)
	{
		if (!eval)
		{
			switch(m_pScheduleData[i].decayType)
			{
			case LINEAR_DECAY:
				m_pParameters->setParameter(m_pScheduleData[i].pName, 
					m_pScheduleData[i].start + progress*(m_pScheduleData[i].end-m_pScheduleData[i].start));
				break;
			case QUADRATIC_DECAY:
				m_pParameters->setParameter(m_pScheduleData[i].pName,
					m_pScheduleData[i].start + (1.- pow(1-progress,2.0))*(m_pScheduleData[i].end-m_pScheduleData[i].start));
				break;
			}
		}
		else
			m_pParameters->setParameter(m_pScheduleData[i].pName,m_pScheduleData[i].evalValue);
	}

}