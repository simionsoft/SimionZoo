#include "stdafx.h"
#include "timer.h"


CTimer::CTimer()
{
	m_startTimePoint = std::chrono::steady_clock::now();
}

void CTimer::startTimer()
{
	m_startTimePoint= std::chrono::steady_clock::now();
}

double CTimer::getElapsedTime(bool resetTimer)
{
	std::chrono::system_clock::time_point timePointNow = std::chrono::steady_clock::now();

	auto timeDiff = timePointNow - m_startTimePoint;

	if (resetTimer) startTimer();

	return (std::chrono::duration<double, std::ratio<1>>(timeDiff)).count();
}