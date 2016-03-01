#include "stdafx.h"
#include "stats.h"

#define MAX_SUBKEY_SIZE 128
CStat::CStat(Stat key, const char* subkey, double* variable)
{
	m_key = key;

	m_subkey = new char[MAX_SUBKEY_SIZE];
	strcpy_s(m_subkey, MAX_SUBKEY_SIZE, subkey);

	m_variable = variable;

	reset();
}

CStat::~CStat()
{
	delete [] m_subkey;
}

void CStat::reset()
{
	m_min = std::numeric_limits<double>::max();
	m_max = std::numeric_limits<double>::min();

	m_numSamples= 0.0;
	m_meanSum= 0.0;
	m_stdDevSum= 0.0;
}

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

void CStat::addSample()
{
	if (!m_variable) return;

	double value = *m_variable;
	m_min = std::min(value, m_min);
	m_max = std::max(value, m_max);

	m_numSamples++;

	double stepSum = value - m_meanSum;
	double stepMean = ((double)(m_numSamples - 1) * stepSum) / (double)m_numSamples;
	m_meanSum += stepMean;
	m_stdDevSum += stepMean * stepSum;
}

double CStat::getMin(){ return m_min; }
double CStat::getMax(){ return m_max; }
double CStat::getAvg(){ return m_meanSum / m_numSamples; }
double CStat::getStdDev(){ return sqrt(fabs(m_stdDevSum / (m_numSamples - 1))); }
double CStat::getValue(){ if (!m_variable) return 0.0; return *m_variable; }
