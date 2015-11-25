#include "stdafx.h"
#include "parameterscheduler.h"
#include "parameters.h"
#include "parameter.h"
#include "globals.h"
#include "experiment.h"

CParameterScheduler::CParameterScheduler(CParameters* pParameters)
{
	char nodeName[MAX_PARAMETER_NAME_SIZE];
	const char *scheduleConfig;
	char strippedName[MAX_PARAMETER_NAME_SIZE];
	char decay[MAX_STRING_SIZE];
	CParameters* pParameterNode;
	CParameters* pScheduleNode;

	if (pParameters && pParameters->exists("NUM_SCHEDULES"))
		m_numParameterSchedules = (int) pParameters->getParameter("NUM_SCHEDULES")->getDouble();
	else
	{
		m_numParameterSchedules = 0;
		m_pScheduleData = 0;
		return;
	}

	m_pParameters = pParameters;		// don't free the memory!!!
	
	m_pScheduleData = new CScheduleData[m_numParameterSchedules];
	CParameters* pRoot = pParameters->getRootNode();

	for (int i = 0; i<m_numParameterSchedules; i++)
	{
		//get the parameter's name from the schedule file
		sprintf_s(nodeName, MAX_PARAMETER_NAME_SIZE, "%d", i);
		pScheduleNode = pParameters->getChild(nodeName);

		//EITHER REWRITE THIS OR GET RID OF THE WHOLE CLASS USING SCHEDULED PARAMETERS WITHIN THE TREE STRUCTURE
		pParameterNode = pRoot->findParent(pScheduleNode->getParameter("PARAMETER")->getStringPtr(), strippedName, false);

		m_pScheduleData[i].pParameter = pParameterNode->getParameter(strippedName);

		//get the schedule line: XMLra pasatzen geanen hau berriz egin behar da!!! eta ondo!!
		scheduleConfig = pScheduleNode->getParameter("SCHEDULE")->getStringPtr();

		//parse the schedule line
		if (sscanf_s(scheduleConfig, "%lf,%lf,%lf,%s", &m_pScheduleData[i].start
			,&m_pScheduleData[i].end, &m_pScheduleData[i].evalValue,decay,MAX_STRING_SIZE)==4)
		{
			if (strcmp(decay,"linear")==0)
				m_pScheduleData[i].decayType= LINEAR_DECAY;
			else if (strcmp(decay,"quadratic")==0)
				m_pScheduleData[i].decayType= QUADRATIC_DECAY;
			else assert(0);
		}
		else
		{
			printf("Error reading parameter schedule :\nLINE %d=%s\n"
				, scheduleConfig);
		}
	}
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
				m_pScheduleData[i].pParameter->setValue(m_pScheduleData[i].start + progress*(m_pScheduleData[i].end-m_pScheduleData[i].start));
				break;
			case QUADRATIC_DECAY:
				m_pScheduleData[i].pParameter->setValue(m_pScheduleData[i].start + (1.- pow(1-progress,2.0))*(m_pScheduleData[i].end-m_pScheduleData[i].start));
				break;
			}
		}
		else
			m_pScheduleData[i].pParameter->setValue(m_pScheduleData[i].evalValue);
	}

}