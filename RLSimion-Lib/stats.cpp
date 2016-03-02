#include "stdafx.h"
#include "stats.h"

#define MAX_KEY_SIZE 128

CStatsInfo::CStatsInfo()
{
	reset();
}

void CStatsInfo::reset()
{
	m_min = std::numeric_limits<double>::max();
	m_max = std::numeric_limits<double>::min();
	m_numSamples = 0;
	m_meanSum = 0;
	m_stdDevSum = 0;
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

void CStatsInfo::addExperimentSample(double value)
{
	m_min = std::min(value, m_min);
	m_max = std::max(value, m_max);

	m_numSamples++;

	double stepSum = value - m_meanSum;
	double stepMean = ((double)(m_numSamples - 1) * stepSum) / (double)m_numSamples;
	m_meanSum += stepMean;
	m_stdDevSum += stepMean * stepSum;
}

double CStatsInfo::getMin(){ return m_min; }
double CStatsInfo::getMax(){ return m_max; }
double CStatsInfo::getAvg(){ return m_meanSum / m_numSamples; }
double CStatsInfo::getStdDev(){ return sqrt(fabs(m_stdDevSum / (m_numSamples - 1))); }



CStats::CStats(const char* key, const char* subkey, double* variable)
{
	m_key = new char[MAX_KEY_SIZE];
	strcpy_s(m_key, MAX_KEY_SIZE, key);

	m_subkey = new char[MAX_KEY_SIZE];
	strcpy_s(m_subkey, MAX_KEY_SIZE, subkey);

	m_variable = variable;

	reset();
}

CStats::~CStats()
{
	delete [] m_key;
	delete [] m_subkey;
}

void CStats::reset()
{
	m_statsInfo.reset();
}

void CStats::addExperimentSample()
{
	if (m_variable)
		m_statsInfo.addExperimentSample(*m_variable);
}

double CStats::getValue()
{
	if (m_variable)
		return *m_variable;
	return 0;
}

CStatsInfo* CStats::getStatsInfo()
{
	return &m_statsInfo;
}

const char* CStats::getKey()
{
	return m_key;
}

const char* CStats::getSubkey()
{
	return m_subkey;
}