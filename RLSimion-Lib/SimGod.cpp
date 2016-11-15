#include "stdafx.h"
#include "SimGod.h"
#include "globals.h"
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

CLASS_INIT(CSimGod)
{
	if (!pParameters) return;

	//the global parameterizations of the state/action spaces
	m_pGlobalStateFeatureMap = CHILD_OBJECT_FACTORY<CStateFeatureMap>(pParameters,"State-Feature-Map",Comment,"The state feature map", Optional,false);
	//CHILD_CLASS_FACTORY(m_pGlobalStateFeatureMap, "State-Feature-Map", "The state feature map", false, CStateFeatureMap);
	m_pGlobalActionFeatureMap = CHILD_OBJECT_FACTORY<CActionFeatureMap>(pParameters, "Action-Feature-Map", Comment, "The state feature map", Optional, TRUE);
	//CHILD_CLASS_FACTORY(m_pGlobalActionFeatureMap, "Action-Feature-Map", "The action feature map", true, CActionFeatureMap);
	m_pExperienceReplay = CHILD_OBJECT<CExperienceReplay>(pParameters, "Experience-Replay", Comment, "The experience replay parameters", Optional, true);
	//CHILD_CLASS(m_pExperienceReplay, "Experience-Replay", "The experience replay parameters", true, CExperienceReplay);
	m_simions= MULTI_VALUE_FACTORY<CSimion>(pParameters,"Simion",Comment, "Simions: learning agents and controllers")
	//MULTI_VALUED_FACTORY(m_simions, "Simion", "Simions: learning agents and controllers", CSimion);

	
	END_CLASS();
}

CSimGod::CSimGod()
{

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

	if (CApp::get()->pExperiment->isEvaluationEpisode()) return;

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