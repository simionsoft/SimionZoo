#pragma once
#include <chrono>
using namespace std;

class CTimer
{
	std::chrono::system_clock::time_point m_startTimePoint;
public:
	CTimer();

	void start();
	double getElapsedTime(bool resetTimer = false);
};