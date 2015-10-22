#include <stdafx.h>
#include "parameterscheduler.h"
#include "parameters.h"
#include "globals.h"
#include "experiment.h"

CParameterScheduler::CParameterScheduler(char *configFile)
{
	char configLine[MAX_STRING_SIZE], *name;
	char decay[MAX_STRING_SIZE];
	CParameters *pParameters= new CParameters(configFile);
	
	int numParameterSchedules= pParameters->getNumParameters();

	m_pScheduleData= new CScheduleData[numParameterSchedules];

	m_numParameterSchedules= 0;

	for (int i= 0; i<numParameterSchedules; i++)
	{
		name= pParameters->getParameterName(i);
		m_pScheduleData[i].pValue= g_pParameters->getDoublePtr(name);

		strcpy_s(configLine, MAX_STRING_SIZE, pParameters->getStringPtr(i));
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
}


CParameterScheduler::~CParameterScheduler()
{
	delete [] m_pScheduleData;
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
				*m_pScheduleData[i].pValue = 
					m_pScheduleData[i].start + progress*(m_pScheduleData[i].end-m_pScheduleData[i].start);
				break;
			case QUADRATIC_DECAY:
				*m_pScheduleData[i].pValue = 
					m_pScheduleData[i].start + (1.- pow(1-progress,2.0))*(m_pScheduleData[i].end-m_pScheduleData[i].start);
				break;
			}
		}
		else
			*m_pScheduleData[i].pValue= m_pScheduleData[i].evalValue;
	}

}