#include "stdafx.h"
#include "experience-replay.h"
#include "app.h"
#include "config.h"
#include "logger.h"
#include "named-var-set.h"
#include "simgod.h"

CExperienceTuple::CExperienceTuple()
{
	s = CSimionApp::get()->pWorld->getDynamicModel()->getStateInstance();
	a = CSimionApp::get()->pWorld->getDynamicModel()->getActionInstance();
	s_p = CSimionApp::get()->pWorld->getDynamicModel()->getStateInstance();
}

void CExperienceTuple::copy(CState* s, CAction* a, CState* s_p, double r, double probability)
{
	this->s->copy(s);
	this->a->copy(a);
	this->s_p->copy(s_p);
	this->r = r;
	this->probability = probability;
}


CExperienceReplay::CExperienceReplay(CConfigNode* pConfigNode)
{
	m_bufferSize = INT_PARAM(pConfigNode, "Buffer-Size", "Size of the buffer used to store experience tuples", 1000);
	m_updateBatchSize = INT_PARAM(pConfigNode, "Update-Batch-Size", "Number of tuples used each time-step in the update", 10);

	CLogger::logMessage(MessageType::Info, "Experience replay buffer initialized");

	m_pTupleBuffer = 0;
	m_currentPosition = 0;
	m_numTuples = 0;
}

CExperienceReplay::CExperienceReplay() : CDeferredLoad()
{
	//default behaviour when experience replay is not used
	m_bufferSize.set(1);
	m_updateBatchSize.set(1);

	CLogger::logMessage(MessageType::Info, "Dummy experience replay buffer used because no config info could be retrieved");

	m_pTupleBuffer = 0;
	m_currentPosition = 0;
	m_numTuples = 0;
}

void CExperienceReplay::deferredLoadStep()
{
	m_pTupleBuffer = new CExperienceTuple[m_bufferSize.get()];
}

CExperienceReplay::~CExperienceReplay()
{
	if (m_pTupleBuffer)
		delete[] m_pTupleBuffer;
}

int CExperienceReplay::getUpdateBatchSize()
{
	return std::min(m_updateBatchSize.get(), m_numTuples);
}

void CExperienceReplay::addTuple(CState* s, CAction* a, CState* s_p, double r, double probability)
{
	//add the experience tuple to the buffer

	if (m_numTuples < m_bufferSize.get())
	{
		//the buffer is not yet full
		m_pTupleBuffer[m_currentPosition].copy(s, a, s_p, r, probability);
		++m_numTuples;
	}
	else
	{
		//the buffer is full
		m_pTupleBuffer[m_currentPosition].copy(s, a, s_p, r, probability);
	}
	m_currentPosition = ++m_currentPosition % m_bufferSize.get();
}

CExperienceTuple* CExperienceReplay::getRandomTupleFromBuffer()
{
	int randomIndex = rand() % m_numTuples;

	return &m_pTupleBuffer[randomIndex];
}