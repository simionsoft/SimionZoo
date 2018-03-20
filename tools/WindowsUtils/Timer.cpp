
#include "Timer.h"


Timer::Timer()
{
	m_startTimePoint = std::chrono::system_clock::now();
}

void Timer::start()
{
	m_startTimePoint= std::chrono::system_clock::now();
}

double Timer::getElapsedTime(bool resetTimer)
{
	std::chrono::system_clock::time_point timePointNow = std::chrono::system_clock::now();

	auto timeDiff = timePointNow - m_startTimePoint;

	if (resetTimer) start();

	return (std::chrono::duration<double, std::ratio<1>>(timeDiff)).count();
}