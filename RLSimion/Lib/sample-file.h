#pragma once

#include <string>
#include <vector>
using namespace std;

class NamedVarSet;
typedef NamedVarSet State;
typedef NamedVarSet Action;
typedef NamedVarSet Reward;

class SampleFile
{
	int m_numSamples = 0;
	int m_sampleSize = 0;
	int m_numElementsPerSample = 0;
	int m_numStateVariables = 0, m_numActionVariables = 0, m_numRewardVariables = 0;
	int m_currentSampleIndex = 0;
	string m_binaryFilename;
	vector<double> m_data;
public:
	SampleFile(string filename);
	~SampleFile();

	void drawRandomSample(State* s, Action* a, State* s_p, double& reward);
};