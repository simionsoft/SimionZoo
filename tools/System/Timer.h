#pragma once
#include <chrono>

class Timer
{
	std::chrono::system_clock::time_point m_startTimePoint;
public:
	Timer();

	void start();
	double getElapsedTime(bool resetTimer = false);
};