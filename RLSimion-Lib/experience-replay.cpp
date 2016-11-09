#include "stdafx.h"
#include "experience-replay.h"
#include "app.h"
#include "globals.h"
#include "config.h"
#include "logger.h"
#include "named-var-set.h"

CExperienceTuple::CExperienceTuple()
{
	s = CApp::get()->pWorld->getDynamicModel()->getStateInstance();
	a = CApp::get()->pWorld->getDynamicModel()->getActionInstance();
	s_p = CApp::get()->pWorld->getDynamicModel()->getStateInstance();
}

void CExperienceTuple::copy(CState* s, CAction* a, CState* s_p, double r)
{
	this->s->copy(s);
	this->a->copy(a);
	this->s_p->copy(s_p);
	this->r = r;
}


CLASS_CONSTRUCTOR(CExperienceReplay) : CDeferredLoad(), CParamObject(pParameters)
{
	if (pParameters)
	{
		CONST_INTEGER_VALUE(m_bufferSize, "Buffer-Size", 1000, "Size of the buffer used to store experience tuples");
		CONST_INTEGER_VALUE(m_updateBatchSize, "Update-Batch-Size", 10, "Number of tuples used each time-step in the update");
	}
	else
	{
		//not very elegant, but simple enough... we want the system to work even if experience replay is not used
		m_bufferSize = 1;
		m_updateBatchSize = 1;
	}
	m_pTupleBuffer = 0;
	m_currentPosition = 0;
	m_numTuples = 0;

	END_CLASS();
}

void CExperienceReplay::deferredLoadStep()
{
	m_pTupleBuffer = new CExperienceTuple[m_bufferSize];
}

CExperienceReplay::~CExperienceReplay()
{
	if (m_pTupleBuffer)
		delete[] m_pTupleBuffer;
}

int CExperienceReplay::getUpdateBatchSize()
{
	return std::min(m_updateBatchSize, m_numTuples);
}

void CExperienceReplay::addTuple(CState* s, CAction* a, CState* s_p, double r)
{
	if (m_bufferSize<=0)
		CApp::get()->pLogger->logMessage(MessageType::Error, "Tried to access the experience replay buffer before initialising it");
	//add the experience tuple to the buffer

	if (m_numTuples < m_bufferSize)
	{
		//the buffer is not yet full
		m_pTupleBuffer[m_currentPosition].copy(s, a, s_p, r);
		++m_numTuples;
	}
	else
	{
		//the buffer is full
		m_pTupleBuffer[m_currentPosition].copy(s, a, s_p, r);
	}
	m_currentPosition = ++m_currentPosition % m_bufferSize;
}

CExperienceTuple* CExperienceReplay::getRandomTupleFromBuffer()
{
	int randomIndex = rand() % m_numTuples;

	return &m_pTupleBuffer[randomIndex];
}