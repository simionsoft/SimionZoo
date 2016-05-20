#pragma once


#include "simion.h"


class CLinearStateActionVFA;
class CQPolicy;
class CQFunctionUpdater;
class CETraces;
class CNumericValue;
class CFeatureList;

////////////////////////////////////////////////////
//Q-function based policies: abstract/factory class
class CQPolicy
{
public:
	static CQPolicy* getInstance(CParameters* pParameters);

	virtual void selectAction(CLinearStateActionVFA* pQFunction, const CState* s, CAction* a)= 0;
};
//Epsilon-greedy policy
class CQEGreedyPolicy : public CQPolicy
{
	CNumericValue* m_pEpsilon;
public:
	CQEGreedyPolicy(CParameters* pParameters);
	virtual ~CQEGreedyPolicy();

	void selectAction(CLinearStateActionVFA* pQFunction, const CState* s, CAction* a);
};
//Soft-max policy
//https://webdocs.cs.ualberta.ca/~sutton/book/2/node4.html

class CQSoftMaxPolicy : public CQPolicy
{

	double *m_pProbabilities;
	CNumericValue* m_pTau;
public:
	CQSoftMaxPolicy(CParameters* pParameters);
	virtual ~CQSoftMaxPolicy();
	void selectAction(CLinearStateActionVFA* pQFunction, const CState* s, CAction* a);
};


/////////////////////////////
//Q-Learning
//https://webdocs.cs.ualberta.ca/~sutton/book/ebook/node78.html

class CQLearning : public CSimion
{
protected:
	CQPolicy* m_pQPolicy;
	CLinearStateActionVFA* m_pQFunction;
	
	CETraces *m_eTraces;
	CNumericValue *m_pAlpha;
	CNumericValue *m_pGamma;
	CFeatureList *m_pAux;
public:
	CQLearning(CParameters* pParameters);
	virtual ~CQLearning();

	//the Q-Function (and thus, the value estimate and the policy) is updated in this method
	virtual void updateValue(const CState *s, const CAction *a, const CState *s_p, double r);

	void selectAction(const CState *s, CAction *a);
	//this method does nothing
	void updatePolicy(const CState *s, const CAction *a, const CState *s_p, double r){};
};

////////////////////////
//SARSA
//https://webdocs.cs.ualberta.ca/~sutton/book/ebook/node77.html

class CSARSA : public CQLearning
{
	CAction *m_nextA;
	bool m_bNextActionSelected;
public:
	CSARSA(CParameters* pParameters);
	~CSARSA();
	void selectAction(const CState *s, CAction *a);
	void updateValue(const CState *s, const CAction *a, const CState *s_p, double r);
};