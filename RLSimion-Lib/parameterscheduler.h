#pragma once

#define LINEAR_DECAY 0
#define QUADRATIC_DECAY 1

class CParameters;

struct CScheduleData
{
	double start, end, evalValue;
	char *pName;
	int decayType;
	
	CScheduleData()
	{
		pName = 0;
	}
	~CScheduleData()
	{
		if (pName) delete [] pName;
	}
};


class CParameterScheduler
{
	CScheduleData *m_pScheduleData;
	int m_numParameterSchedules;
	CParameters *m_pParameters;

public:
	CParameterScheduler(char* configFile, CParameters* pParameters);
	~CParameterScheduler();

	void update();
};