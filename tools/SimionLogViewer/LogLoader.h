#pragma once

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


class ExperimentLog
{
	ExperimentHeader m_header;
	Episode *m_pEpisodes = 0;
	vector<string> m_variableNames;

public:
	//xml tags
	const char* xmlTagBinaryDataFile = "BinaryDataFile";
	const char* xmlTagSceneFile = "SceneFile";

	ExperimentLog() { }
	~ExperimentLog();

	int getNumEpisodes() const { return (int) m_header.numEpisodes; }
	Episode* getEpisode(int i) { return &m_pEpisodes[i]; }

	int getVariableIndex(string variableName) const;

	bool load(string descriptorFile, string& outSceneFile);
};
