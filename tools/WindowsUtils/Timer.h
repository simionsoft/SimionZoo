#pragma once
#include <chrono>

class CTimer
{
	std::chrono::system_clock::time_point m_startTimePoint;
public:
	CTimer();

	void start();
	double getElapsedTime(bool resetTimer = false);
};