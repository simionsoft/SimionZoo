#pragma once

#include <vector>
#include "parameterized-object.h"

enum Stat{State,Action,TDError,ExplorationNoise,VectorSize,Reward};

class CStat
{
	double m_min;
	double m_max;
	double m_numSamples;
	double m_meanSum;
	double m_stdDevSum;

	Stat m_key;
	char* m_subkey;
	double* m_variable;

public:
	CStat(Stat key, const char* subkey, double *variable);
	~CStat();

	const char* getKey();
	const char* getSubkey();

	void reset();
	void addSample();

	double getValue(); //the value in the last step

	double getMin();
	double getMax();
	double getAvg();
	double getStdDev();
};

