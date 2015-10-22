#pragma once

#define LINEAR_DECAY 0
#define QUADRATIC_DECAY 1


struct CScheduleData
{
	double start, end, evalValue;
	double *pValue;
	int decayType;
};


class CParameterScheduler
{
	CScheduleData *m_pScheduleData;
	int m_numParameterSchedules;

public:
	CParameterScheduler(char* configFile);
	~CParameterScheduler();

	void update();
};