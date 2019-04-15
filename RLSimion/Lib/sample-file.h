#pragma once

#include <stdio.h>
#include <string>
using namespace std;

class NamedVarSet;
typedef NamedVarSet State;
typedef NamedVarSet Action;
typedef NamedVarSet Reward;

class SampleFile
{
	int m_numSamples = 0;
	int m_sampleSize = 0;
	FILE* m_pBinaryFile = nullptr;
	int m_numStateVariables = 0, m_numActionVariables = 0, m_numRewardVariables = 0;
	string m_binaryFilename;
public:
	SampleFile(string filename);
	~SampleFile();

	void drawRandomSample(State* s, Action* a, State* s_p, double& reward);
};