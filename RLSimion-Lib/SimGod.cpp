#include "stdafx.h"
#include "SimGod.h"
#include "globals.h"
#include "experiment.h"
#include "named-var-set.h"
#include "parameters.h"
#include "simion.h"
#include "app.h"
#include "delayed-load.h"
#include "utils.h"

CLASS_INIT(CSimGod)
{
	if (!pParameters) return;
	m_numSimions = pParameters->countChildren("Simion");
	m_pSimions = new CSimion*[m_numSimions];
	CParameters* pChild = pParameters->getChild("Simion");
	for (int i = 0; i < m_numSimions; i++)
	{
		m_pSimions[i] = 0;
		MULTI_VALUED_FACTORY(m_pSimions[i], "Simion", "Simions: agents and controllers",CSimion, pChild);
		pChild = pChild->getNextChild("Simion");
	}
	
	//m_td = 0.0;
	//CApp::Logger.addVarToStats("Critic", "TD-error", &m_td);
	END_CLASS();
}

CSimGod::CSimGod()
{
	m_numSimions = 0;
	m_pSimions = 0;
}


CSimGod::~CSimGod()
{
	if (m_pSimions)
	{
		for (int i = 0; i < m_numSimions; i++)
		{
			if (m_pSimions[i]) delete m_pSimions[i];
		}
		delete[] m_pSimions;
	}
	for (auto it = m_inputFiles.begin(); it != m_inputFiles.end(); it++) delete (*it);
	for (auto it = m_outputFiles.begin(); it != m_outputFiles.end(); it++) delete (*it);
}


void CSimGod::selectAction(CState* s, CAction* a)
{
	for (int i = 0; i < m_numSimions; i++)
		m_pSimions[i]->selectAction(s, a);
}

void CSimGod::update(CState* s, CAction* a, CState* s_p, double r)
{
	if (CApp::Experiment.isEvaluationEpisode()) return;

	//update critic
	for (int i = 0; i < m_numSimions; i++)
		m_pSimions[i]->updateValue(s, a,s_p,r);

	//update actor: might be the controller
	for (int i = 0; i < m_numSimions; i++)
		m_pSimions[i]->updatePolicy(s, a,s_p,r);
}

void CSimGod::registerDelayedLoadObj(CDeferredLoad* pObj)
{
	m_delayedLoadObjects.push_back(pObj);
}

void CSimGod::delayedLoad()
{
	for (auto it = m_delayedLoadObjects.begin(); it != m_delayedLoadObjects.end(); it++)
	{
		(*it)->deferredLoadStep();
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