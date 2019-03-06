/*
	SimionZoo: A framework for online model-free Reinforcement Learning on continuous
	control problems

	Copyright (c) 2016 SimionSoft. https://github.com/simionsoft

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#include "stats.h"
#include <algorithm>
#include <math.h>

#define MAX_KEY_SIZE 128

StatsInfo::StatsInfo()
{
	reset();
}

/// <summary>
/// Resets the stats
/// </summary>
void StatsInfo::reset()
{
	m_min = std::numeric_limits<double>::max();
	m_max = std::numeric_limits<double>::lowest();
	m_numSamples = 0;
	m_meanSum = 0.0;
	m_stdDevSum = 0.0;
	m_sum = 0.0;
}


//one pass-mean/std.dev. calculation
//https://www.strchr.com/standard_deviation_in_one_pass
//double std_dev(double a[], int n) {
//	if (n == 0)
//		return 0.0;
//	int i = 0;
//	double meanSum = a[0];
//	double stdDevSum = 0.0;
//	for (i = 1; i < n; ++i) {
//		double stepSum = a[i] - meanSum;
//		double stepMean = ((i - 1) * stepSum) / i;
//		meanSum += stepMean;
//		stdDevSum += stepMean * stepSum;
//	}
//	// for poulation variance: return sqrt(stdDevSum / elements);
//	return sqrt(stdDevSum / (elements - 1));
//}


/// <summary>
/// Adds a sample to the collection of values
/// </summary>
/// <param name="value">New sample</param>
void StatsInfo::addSample(double value)
{
	m_min = std::min(value, m_min);
	m_max = std::max(value, m_max);

	m_numSamples++;

	double stepSum = value - m_meanSum;
	double stepMean = ((double)(m_numSamples - 1) * stepSum) / (double)m_numSamples;
	m_meanSum += stepMean;
	m_sum += value;
	m_stdDevSum += stepMean * stepSum;
}

/// <summary>
/// Returns the maximum sampled value
/// </summary>
double StatsInfo::getMin(){ return m_min; }

/// <summary>
/// Returns the minimum sampled value
/// </summary>
double StatsInfo::getMax(){ return m_max; }

/// <summary>
/// Returns the mean value of all samples
/// </summary>
double StatsInfo::getAvg()
{
	if (m_numSamples > 0) return m_sum / m_numSamples;
	return 0.0;
}

/// <summary>
/// Returns the standard deviation of all the samples
/// </summary>
double StatsInfo::getStdDev()
{
	if (m_numSamples > 1) return sqrt(fabs(m_stdDevSum / (m_numSamples - 1)));
	return 0.0;
}
