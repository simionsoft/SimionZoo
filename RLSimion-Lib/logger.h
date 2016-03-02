#pragma once

class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;
typedef CNamedVarSet CReward;


class tinyxml2::XMLElement;

#include <vector>
#include "stats.h"

class CLogger
{
	
	char *m_outputDir;
	char *m_filePrefix;

	void *m_pFile;

	bool m_bLogEvaluationEpisodes;
	bool m_bLogTrainingEpisodes;
	bool m_bLogEvaluationSummary;
	double m_logFreq; //in seconds

	__int64 m_lastCounter;
	__int64 m_counterFreq;

	double m_lastLogTime;
	double m_episodeRewards;
	double m_lastEvaluationAvgReward;

	//regular log files
	void openEpisodeLogFile();
	void writeEpisodeStep(CState *s, CAction *a, CState *s_p, CReward *pReward);
	void writeEpisodeLogFileHeader(CState *s, CAction *a, CReward *pReward);

	//summary files: avg reward of evaluation episodes
	void writeEpisodeSummary();

	bool isCurrentEpisodeLogged();

	//stats
	std::vector<CStat*> m_stats;
public:
	CLogger(tinyxml2::XMLElement* pParameters);
	~CLogger();

	void addVarToStats(Stat key, const char* subkey, double* variable);
	void addVarSetToStats(Stat key, CNamedVarSet* varset);

	void timestep(CState *s, CAction *a,CState *s_p, CReward* r);
};