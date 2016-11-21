#include "stdafx.h"
#include "SimGod.h"
#include "experiment.h"
#include "named-var-set.h"
#include "config.h"
#include "simion.h"
#include "app.h"
#include "delayed-load.h"
#include "utils.h"
#include "featuremap.h"
#include "experience-replay.h"
#include "parameters.h"

std::vector<std::pair<CDeferredLoad*, unsigned int>> CSimGod::m_delayedLoadObjects;
std::vector<const char*> CSimGod::m_inputFiles;
std::vector<const char*> CSimGod::m_outputFiles;
CHILD_OBJECT_FACTORY<CStateFeatureMap> CSimGod::m_pGlobalStateFeatureMap;
CHILD_OBJECT_FACTORY<CActionFeatureMap> CSimGod::m_pGlobalActionFeatureMap;

CSimGod::CSimGod(CConfigNode* pConfigNode)
{
	if (!pConfigNode) return;

	//the global parameterizations of the state/action spaces
	m_pGlobalStateFeatureMap = CHILD_OBJECT_FACTORY<CStateFeatureMap>(pConfigNode,"State-Feature-Map","The state feature map");
	m_pGlobalActionFeatureMap = CHILD_OBJECT_FACTORY<CActionFeatureMap>(pConfigNode, "Action-Feature-Map", "The state feature map", true);
	m_pExperienceReplay = CHILD_OBJECT<CExperienceReplay>(pConfigNode, "Experience-Replay", "The experience replay parameters", true);
	m_simions = MULTI_VALUE_FACTORY<CSimion>(pConfigNode, "Simion", "Simions: learning agents and controllers");
}


CSimGod::~CSimGod()
{
	for (auto it = m_inputFiles.begin(); it != m_inputFiles.end(); it++) delete (*it);
	m_inputFiles.clear();
	for (auto it = m_outputFiles.begin(); it != m_outputFiles.end(); it++) delete (*it);
	m_outputFiles.clear();
}


void CSimGod::selectAction(CState* s, CAction* a)
{
	for (unsigned int i = 0; i < m_simions.size(); i++)
		m_simions[i]->selectAction(s, a);
}

void CSimGod::update(CState* s, CAction* a, CState* s_p, double r)
{
	CExperienceTuple* pExperienceTuple;

	if (CSimionApp::get()->pExperiment->isEvaluationEpisode()) return;

	m_pExperienceReplay->addTuple(s, a, s_p, r);

	int updateBatchSize = m_pExperienceReplay->getUpdateBatchSize();
	for (int tuple = 0; tuple < updateBatchSize; ++tuple)
	{
		pExperienceTuple = m_pExperienceReplay->getRandomTupleFromBuffer();
		//update critic
		for (unsigned int i = 0; i < m_simions.size(); i++)
			m_simions[i]->updateValue(pExperienceTuple->s, pExperienceTuple->a, pExperienceTuple->s_p, pExperienceTuple->r);

		//update actor: might be the controller
		for (unsigned int i = 0; i < m_simions.size(); i++)
			m_simions[i]->updatePolicy(pExperienceTuple->s, pExperienceTuple->a, pExperienceTuple->s_p, pExperienceTuple->r);
	}
}

void CSimGod::registerDelayedLoadObj(CDeferredLoad* pObj,unsigned int loadOrder)
{
	m_delayedLoadObjects.push_back(std::pair<CDeferredLoad*,unsigned int>(pObj,loadOrder));
}

bool myComparison(const std::pair<CDeferredLoad*, unsigned int> &a, const std::pair<CDeferredLoad*, unsigned int> &b)
{
	return a.second<b.second;
}

void CSimGod::delayedLoad()
{
	std::sort(m_delayedLoadObjects.begin(), m_delayedLoadObjects.end(),myComparison);

	for (auto it = m_delayedLoadObjects.begin(); it != m_delayedLoadObjects.end(); it++)
	{
		(*it).first->deferredLoadStep();
	}
}

void CSimGod::registerInputFile(const char* filepath)
{
	char* copy = new char[strlen(filepath) + 1];
	strcpy_s(copy, strlen(filepath) + 1, filepath);
	m_inputFiles.push_back(copy);
}

void CSimGod::getInputFiles(CFilePathList& filepathList)
{
	for (auto it = m_inputFiles.begin(); it != m_inputFiles.end(); it++)
	{
		filepathList.addFilePath(*it);
	}
}

void CSimGod::registerOutputFile(const char* filepath)
{
	char* copy = new char[strlen(filepath) + 1];
	strcpy_s(copy, strlen(filepath) + 1, filepath);
	m_outputFiles.push_back(copy);
}

void CSimGod::getOutputFiles(CFilePathList& filepathList)
{
	for (auto it = m_outputFiles.begin(); it != m_outputFiles.end(); it++)
	{
		filepathList.addFilePath(*it);
	}
}

std::shared_ptr<CStateFeatureMap> CSimGod::getGlobalStateFeatureMap()
{ 
	return m_pGlobalStateFeatureMap.shared_ptr(); 
}
std::shared_ptr<CActionFeatureMap> CSimGod::getGlobalActionFeatureMap()
{
	return m_pGlobalActionFeatureMap.shared_ptr(); 
}