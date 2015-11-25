#pragma once

#define LINEAR_DECAY 0
#define QUADRATIC_DECAY 1

class CParameters;
class CParameter;

struct CScheduleData
{
	double start, end, evalValue;
	CParameter* pParameter;
	int decayType;
	
	CScheduleData()
	{
	}
	~CScheduleData()
	{
	}
};


class CParameterScheduler
{
	CScheduleData *m_pScheduleData;
	int m_numParameterSchedules;
	CParameters *m_pParameters;

public:
	CParameterScheduler(CParameters* pParameters);
	~CParameterScheduler();

	void update();
};