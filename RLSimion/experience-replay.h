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
	INT_PARAM m_blockSize;
	INT_PARAM m_updateBatchSize;

	int m_currentPosition= 0;
	int m_numTuples= 0;

public:
	ExperienceReplay(ConfigNode* pParameters);
	ExperienceReplay();
	~ExperienceReplay();

	bool bUsing();

	void addTuple(const State* s, const Action* a, const State* s_p, double r, double probability);
	int getUpdateBatchSize();
	int getMaxUpdateBatchSize() { return m_updateBatchSize.get(); }
	ExperienceTuple* getRandomTupleFromBuffer();

	void deferredLoadStep();
};