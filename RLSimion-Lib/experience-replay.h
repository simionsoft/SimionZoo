#pragma once

#include "delayed-load.h"
#include "parameters.h"
class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;
class CConfigNode;

class CExperienceTuple
{
public:
	CState* s;
	CAction* a;
	CState* s_p;
	double r;
	double probability; //probability under which the actor took action a in state s

	CExperienceTuple();
	void copy(CState* s, CAction* a, CState* s_p, double r,double probability);
};

class CExperienceReplay: public CDeferredLoad
{
	CExperienceTuple* m_pTupleBuffer;
	INT_PARAM m_bufferSize;
	INT_PARAM m_updateBatchSize;

	int m_currentPosition= 0;
	int m_numTuples= 0;

public:
	CExperienceReplay(CConfigNode* pParameters);
	CExperienceReplay();
	~CExperienceReplay();

	void addTuple(CState* s, CAction* a, CState* s_p, double r, double probability);
	int getUpdateBatchSize();
	CExperienceTuple* getRandomTupleFromBuffer();

	void deferredLoadStep();
};