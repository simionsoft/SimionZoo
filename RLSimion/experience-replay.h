#pragma once

#include "deferred-load.h"
#include "parameters.h"
class NamedVarSet;
typedef NamedVarSet State;
typedef NamedVarSet Action;
class ConfigNode;

class ExperienceTuple
{
public:
	State* s;
	Action* a;
	State* s_p;
	double r;
	double probability; //probability under which the actor took action a in state s

	ExperienceTuple();
	void copy(const State* s, const Action* a, const  State* s_p, double r,double probability);
};

class ExperienceReplay: public DeferredLoad
{
	ExperienceTuple* m_pTupleBuffer;
	INT_PARAM m_bufferSize;
	INT_PARAM m_updateBatchSize;

	size_t m_currentPosition= 0;
	size_t m_numTuples= 0;
	const unsigned int m_minUpdateSizeTimes = 4; //how many update-size times tuples we need to start updating
public:
	ExperienceReplay(ConfigNode* pParameters);
	ExperienceReplay();
	~ExperienceReplay();

	bool bUsing();
	bool bHaveEnoughTuples() const;

	void addTuple(const State* s, const Action* a, const State* s_p, double r, double probability);
	size_t getUpdateBatchSize() const;
	ExperienceTuple* getRandomTupleFromBuffer();

	void deferredLoadStep();
};