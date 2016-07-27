#pragma once

#include "parameterized-object.h"
#include "delayed-load.h"

class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;

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

class CExperienceReplay: public CParamObject, public CDeferredLoad
{
	CExperienceTuple* m_pTupleBuffer;
	int m_bufferSize;
	int m_updateBatchSize;

	int m_currentPosition;
	int m_numTuples;

public:
	CExperienceReplay(CParameters* pParameters);
	~CExperienceReplay();

	void addTuple(CState* s, CAction* a, CState* s_p, double r);
	int getUpdateBatchSize(){ return m_updateBatchSize; }
	CExperienceTuple* getRandomTupleFromBuffer();

	void deferredLoadStep();
};