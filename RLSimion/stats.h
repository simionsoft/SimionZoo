#pragma once

#include <vector>
#include <string>
using namespace std;

class CStatsInfo
{
	double m_min;
	double m_max;
	double m_numSamples;
	double m_sum;
	double m_meanSum;
	double m_stdDevSum;

	bool m_bLog;
public:
	CStatsInfo();

	void reset();

	double getMin();
	double getMax();
	double getAvg();
	double getStdDev();

	void addSample(double value);
};

//enum DataType{Double, Int, UnsignedInt};

//common interface
class IStats
{
protected:
	CStatsInfo m_statsInfo;
	string m_key;
	string m_subkey;
public:
	CStatsInfo* getStatsInfo() { return &m_statsInfo; }
	string getKey() { return m_key; }
	string getSubkey() { return m_subkey; }
	void reset() { m_statsInfo.reset(); }

	virtual void addSample() = 0;
	virtual double get() = 0;
};

template <typename T>
class CStats: public IStats
{
	T& m_variable;
public:
	template <typename T>
	CStats(string key, string subkey, T& variable): m_variable(variable)
	{
		m_key = key;
		m_subkey = subkey;
		reset();
	}
	virtual ~CStats() {}

	virtual void addSample() { m_statsInfo.addSample(m_variable); }
	virtual double get() { return (double)m_variable; }
};

