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

void CStatsInfo::addExperimentSample(double value)
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

double CStatsInfo::getMin(){ return m_min; }
double CStatsInfo::getMax(){ return m_max; }
double CStatsInfo::getAvg()
{
	if (m_numSamples > 0) return m_sum / m_numSamples;
	return 0.0;
}
double CStatsInfo::getStdDev()
{
	if (m_numSamples > 1) return sqrt(fabs(m_stdDevSum / (m_numSamples - 1)));
	return 0.0;
}



CStats::CStats(const char* key, const char* subkey, const void* variable, DataType type)
{
	m_key = new char[MAX_KEY_SIZE];
	strcpy_s(m_key, MAX_KEY_SIZE, key);

	m_subkey = new char[MAX_KEY_SIZE];
	strcpy_s(m_subkey, MAX_KEY_SIZE, subkey);

	m_variable = (void*) variable;
	m_type = type;

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
	{
		if (m_type==Double)
			m_statsInfo.addExperimentSample(*(double*)m_variable);
		else if (m_type==UnsignedInt)
			m_statsInfo.addExperimentSample(*(unsigned int*)m_variable);
		else  if (m_type == Int)
			m_statsInfo.addExperimentSample(*(int*)m_variable);
	}
	assert(0);
}

double CStats::getValue()
{
	if (m_variable)
	{
		if (m_type == Double)
			return *(double*)m_variable;
		else if (m_type==UnsignedInt)
			return (double)*(unsigned int*)m_variable;
		else if (m_type == Int)
			return (double)*(unsigned int*)m_variable;
	}
	assert(0);
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