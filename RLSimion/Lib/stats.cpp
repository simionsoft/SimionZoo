#include "stats.h"
#include <algorithm>
#include <math.h>

#define MAX_KEY_SIZE 128

StatsInfo::StatsInfo()
{
	reset();
}

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

double StatsInfo::getMin(){ return m_min; }
double StatsInfo::getMax(){ return m_max; }
double StatsInfo::getAvg()
{
	if (m_numSamples > 0) return m_sum / m_numSamples;
	return 0.0;
}
double StatsInfo::getStdDev()
{
	if (m_numSamples > 1) return sqrt(fabs(m_stdDevSum / (m_numSamples - 1)));
	return 0.0;
}
