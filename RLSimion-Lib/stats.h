#pragma once

#include <vector>
#include "parameterized-object.h"





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
	bool log();

	void reset();
	double getValue(); //the value in the last step

	double getMin();
	double getMax();
	double getAvg();
	double getStdDev();

	void addExperimentSample(double value);
};

enum DataType{Double, Int, UnsignedInt};

class CStats
{
	CStatsInfo m_statsInfo;

	char* m_key;
	char* m_subkey;
	void* m_variable;
	DataType m_type;
public:
	CStats(const char* key, const char* subkey, const void *variable, DataType type);
	virtual ~CStats();

	void reset();

	const char* getKey();
	const char* getSubkey();
	double getValue();

	CStatsInfo* getStatsInfo();

	void addExperimentSample();
};

