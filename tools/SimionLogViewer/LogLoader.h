#pragma once
#include "../../RLSimion/named-var-set.h"
#include <vector>
#include <string>
using namespace std;

#define HEADER_MAX_SIZE 16
#define EXPERIMENT_HEADER 1
#define EPISODE_HEADER 2
#define STEP_HEADER 3
#define EPISODE_END_HEADER 4

struct StepHeader
{
	__int64 magicNumber = STEP_HEADER;
	__int64 stepIndex;

	double experimentRealTime;
	double m_episodeSimTime;
	double episodeRealTime;

	__int64 padding[HEADER_MAX_SIZE - 5]; //extra space
	StepHeader()
	{
		memset(padding, 0, sizeof(padding));
	}
};

class Step
{
	StepHeader m_header;
	int m_numValues;
	double *m_pValues;
public:
	Step(int numVars);
	~Step();

	int getNumValues() { return m_numValues; }
	double getValue(int i) const;
	void setValue(int i, double value);

	double getExperimentRealTime() const;
	double getEpisodeSimTime() const;
	double getEpisodeRealTime() const;

	void load(FILE* pFile);
	bool bEnd();
};

struct EpisodeHeader
{
	__int64 magicNumber = EPISODE_HEADER;
	__int64 episodeType;
	__int64 episodeIndex;
	__int64 numVariablesLogged;

	//Added in version 2: if the m_episodeIndex belongs to an evaluation, the number of episodes per evaluation might be >1
	//the episodeSubIndex will be in [1..numEpisodesPerEvaluation]
	__int64 episodeSubIndex;

	__int64 padding[HEADER_MAX_SIZE - 5]; //extra space
	EpisodeHeader()
	{
		memset(padding, 0, sizeof(padding));
	}
};

class Episode
{
	EpisodeHeader m_header;
	vector<Step*> m_pSteps;
public:
	Episode() { }
	~Episode();

	size_t getNumSteps() const { return m_pSteps.size(); }
	Step* getStep(int i);
	int getNumValuesPerStep() const { if (m_pSteps.size() == 0) return 0; return m_pSteps[0]->getNumValues(); }
	double getSimTimeLength()const { if (m_pSteps.size() == 0) return 0.0; return m_pSteps[m_pSteps.size() - 1]->getEpisodeSimTime(); }
	void load(FILE* pFile);
};


struct ExperimentHeader
{
	__int64 magicNumber = EXPERIMENT_HEADER;
	__int64 fileVersion = 0;
	__int64 numEpisodes = 0;

	__int64 padding[HEADER_MAX_SIZE - 3]; //extra space
	ExperimentHeader()
	{
		memset(padding, 0, sizeof(padding));
	}
};




#define MAX_FUNCTION_ID_LENGTH 128

//using __int64 to assure C# data-type compatibility

struct FunctionLogHeader
{
	__int64 magicNumber;
	__int64 fileVersion;
	__int64 numFunctions = 0;
};


struct FunctionDeclarationHeader
{
	__int64 magicNumber;
	__int64 id;
	char name[MAX_FUNCTION_ID_LENGTH];
	__int64 numSamplesX;
	__int64 numSamplesY;
	__int64 numSamplesZ;
};



struct FunctionSampleHeader
{
	__int64 magicNumber;
	__int64 episode;
	__int64 step;
	__int64 experimentStep;
	__int64 id;
};

class FunctionSample
{
	size_t m_episode, m_step, m_experimentStep;
	vector<double> m_values;
public:
	FunctionSample(size_t episode, size_t step, size_t experimentStep, size_t numSamples);
	~FunctionSample();

	vector<double>& values() { return m_values; }
	double value(size_t index) { return m_values[index]; }
	size_t episode() { return m_episode; }
	size_t step() { return m_step; }
	size_t experimentStep() { return m_experimentStep; }
};

class Function
{
	vector<FunctionSample*> m_samples;
	size_t m_numSamplesX = 0;
	size_t m_numSamplesY = 0;
	size_t m_numSamplesZ = 0;
	string m_name;

	int m_lastInterpolatedEpisode = -1; //set -1 to mark it as "not yet done"
	vector<double> m_interpolatedValues;
public:
	Function(string name, size_t numSamplesX, size_t numSamplesY, size_t numSamplesZ);
	~Function();

	string name() const { return m_name; }
	size_t numSamples() const { return m_numSamplesX * m_numSamplesY ; } //we don't consider z for now
	size_t numSamplesX() const { return m_numSamplesX; }
	size_t numSamplesY() const { return m_numSamplesY; }
	size_t numSamplesZ() const { return m_numSamplesZ; }
	const vector<double>& getInterpolatedData(size_t episode, size_t step, bool &dataChanged);
	void addSample(FunctionSample* functionSample) { m_samples.push_back(functionSample); }
};

class FunctionLog
{
	vector<Function*> m_functions;
public:
	FunctionLog() {}
	~FunctionLog();
	void load(const char* functionLogFile);
	vector<Function*> const getFunctions() const { return m_functions; };
};


class ExperimentLog
{
	ExperimentHeader m_header;
	Episode *m_pEpisodes = 0;

	Descriptor m_descriptor;

	FunctionLog m_functionLog;
public:
	//xml tags
	const char* xmlTagBinaryDataFile = "BinaryDataFile";
	const char* xmlTagSceneFile = "SceneFile";
	const char* xmlTagFunctionLogFile = "FunctionsDataFile";

	ExperimentLog() { }
	~ExperimentLog();

	int getNumEpisodes() const { return (int) m_header.numEpisodes; }
	Episode* getEpisode(int i) { return &m_pEpisodes[i]; }

	int getVariableIndex(string variableName) const;

	bool load(string descriptorFile, string& outSceneFile);

	Descriptor& getDescriptor() { return m_descriptor; }

	FunctionLog& getFunctionLog() { return m_functionLog; }
};
