
#include "Timer.h"


CTimer::CTimer()
{
	m_startTimePoint = std::chrono::system_clock::now();
}

void CTimer::start()
{
	m_startTimePoint= std::chrono::system_clock::now();
}

double CTimer::getElapsedTime(bool resetTimer)
{
	std::chrono::system_clock::time_point timePointNow = std::chrono::system_clock::now();

	auto timeDiff = timePointNow - m_startTimePoint;

	if (resetTimer) start();

	return (std::chrono::duration<double, std::ratio<1>>(timeDiff)).count();
}