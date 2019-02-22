/*
	SimionZoo: A framework for online model-free Reinforcement Learning on continuous
	control problems

	Copyright (c) 2016 SimionSoft. https://github.com/simionsoft

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#include "experience-replay.h"
#include "app.h"
#include "config.h"
#include "logger.h"
#include "../Common/named-var-set.h"
#include "simgod.h"
#include "worlds/world.h"
#include <algorithm>

ExperienceTuple::ExperienceTuple()
{
	s = SimionApp::get()->pWorld->getDynamicModel()->getStateInstance();
	a = SimionApp::get()->pWorld->getDynamicModel()->getActionInstance();
	s_p = SimionApp::get()->pWorld->getDynamicModel()->getStateInstance();
}

void ExperienceTuple::copy(const State* s, const Action* a, const State* s_p, double r, double probability)
{
	this->s->copy(s);
	this->a->copy(a);
	this->s_p->copy(s_p);
	this->r = r;
	this->probability = probability;
}


ExperienceReplay::ExperienceReplay(ConfigNode* pConfigNode)
{
	m_bufferSize = INT_PARAM(pConfigNode, "Buffer-Size", "Size of the buffer used to store experience tuples", 1000);
	m_updateBatchSize = INT_PARAM(pConfigNode, "Update-Batch-Size", "Number of tuples used each time-step in the update", 10);

	Logger::logMessage(MessageType::Info, "Experience replay buffer initialized");

	m_pTupleBuffer = 0;
	m_currentPosition = 0;
	m_numTuples = 0;
}

ExperienceReplay::ExperienceReplay() : DeferredLoad()
{
	//default behaviour when experience replay is not used
	m_bufferSize.set(0);
	m_updateBatchSize.set(0);

	m_pTupleBuffer = 0;
	m_currentPosition = 0;
	m_numTuples = 0;
}

bool ExperienceReplay::bUsing()
{
	return m_bufferSize.get() != 0;
}

void ExperienceReplay::deferredLoadStep()
{
	m_pTupleBuffer = new ExperienceTuple[m_bufferSize.get()];
}

ExperienceReplay::~ExperienceReplay()
{
	if (m_pTupleBuffer)
		delete[] m_pTupleBuffer;
}

size_t ExperienceReplay::getUpdateBatchSize() const
{
	return m_updateBatchSize.get();
}

bool ExperienceReplay::bHaveEnoughTuples() const
{
	size_t minNumTuplesForUpdate = 
		std::min((size_t)m_bufferSize.get()
			, (size_t)m_minUpdateSizeTimes* m_updateBatchSize.get());
	return m_numTuples >= minNumTuplesForUpdate;
}

void ExperienceReplay::addTuple(const State* s, const  Action* a, const State* s_p, double r, double probability)
{
	//add the experience tuple to the buffer
	if (!bUsing()) return;

	if (m_numTuples < (size_t)m_bufferSize.get())
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
	m_currentPosition = ++m_currentPosition % (size_t) m_bufferSize.get();
}

ExperienceTuple* ExperienceReplay::getRandomTupleFromBuffer()
{
	int randomIndex = rand() % (size_t) m_numTuples;

	return &m_pTupleBuffer[randomIndex];
}