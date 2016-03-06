#pragma once

class CTimer
{
	std::chrono::system_clock::time_point m_startTimePoint;
public:
	CTimer();

	void startTimer();
	double getElapsedTime(bool resetTimer = false);
};