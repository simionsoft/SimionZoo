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


	CExperienceTuple();
	void copy(CState* s, CAction* a, CState* s_p, double r);
};

class CExperienceReplay: public CDeferredLoad
{
	CExperienceTuple* m_pTupleBuffer;
	INT_PARAM m_bufferSize;
	INT_PARAM m_updateBatchSize;

	int m_currentPosition;
	int m_numTuples;

public:
	CExperienceReplay(CConfigNode* pParameters);
	~CExperienceReplay();

	void addTuple(CState* s, CAction* a, CState* s_p, double r);
	int getUpdateBatchSize();
	CExperienceTuple* getRandomTupleFromBuffer();

	void deferredLoadStep();
};