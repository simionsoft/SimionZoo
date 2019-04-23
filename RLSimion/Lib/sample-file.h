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
	int m_numSamplesInFile = 0;
	int m_sampleSizeInBytes = 0;
	int m_numElementsPerSample = 0;
	int m_numStateVariables = 0, m_numActionVariables = 0, m_numRewardVariables = 0;
	int m_currentSampleIndex = 0;
	string m_binaryFilename;

	vector<double> m_cachedData;
	int m_dataChunkSizeInSamples;
	int m_dataChunkSizeInElements;
	int m_numValidSamplesInCache;
	int m_numChunksInFile;
	int m_currentChunk;

	void* m_pBinaryFile;

	void loadNextDataChunkFromFile();
public:
	SampleFile(string filename);
	~SampleFile();

	void drawRandomSample(State* s, Action* a, State* s_p, double& reward);
};