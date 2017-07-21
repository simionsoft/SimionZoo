#pragma once


#include "simion.h"
#include "critic.h"

class CLinearStateActionVFA;
class CQPolicy;
class CQFunctionUpdater;
class CETraces;
class CNumericValue;
class CFeatureList;

#include "parameters.h"

////////////////////////////////////////////////////
//Q-function based policies: abstract/factory class
class CQPolicy
{
public:
	virtual ~CQPolicy(){}
	static std::shared_ptr<CQPolicy> getInstance(CConfigNode* pParameters);

	virtual double selectAction(CLinearStateActionVFA* pQFunction, const CState* s, CAction* a)= 0;
};
//Epsilon-greedy policy
class CQEGreedyPolicy : public CQPolicy
{
	CHILD_OBJECT_FACTORY<CNumericValue> m_pEpsilon;
public:
	CQEGreedyPolicy(CConfigNode* pParameters);
	virtual ~CQEGreedyPolicy();

	double selectAction(CLinearStateActionVFA* pQFunction, const CState* s, CAction* a);
};
//Soft-max policy
//https://webdocs.cs.ualberta.ca/~sutton/book/2/node4.html

class CQSoftMaxPolicy : public CQPolicy
{

	double *m_pProbabilities;
	CHILD_OBJECT_FACTORY<CNumericValue> m_pTau;
public:
	CQSoftMaxPolicy(CConfigNode* pParameters);
	virtual ~CQSoftMaxPolicy();
	double selectAction(CLinearStateActionVFA* pQFunction, const CState* s, CAction* a);
};


/////////////////////////////
//Q-Learning
//https://webdocs.cs.ualberta.ca/~sutton/book/ebook/node78.html

class CQLearningCritic : public ICritic
{
protected:
	//if this variable is true: td= r + gamma*max a' Q(s_p,a') - max a' Q(s,a')
	//else: td= r + gamma* max a' Q(s_p,a') - Q(s,a)
	bool m_bUseVFunctionAsBaseline = true;

	CHILD_OBJECT<CLinearStateActionVFA> m_pQFunction;
	CHILD_OBJECT_FACTORY<CNumericValue> m_pAlpha;
	CFeatureList *m_pAux;
	CHILD_OBJECT<CETraces> m_eTraces;
public:
	CQLearningCritic(CConfigNode* pParameters);
	virtual ~CQLearningCritic();
	//the Q-Function is updated in this method (and thus, any policy derived from it such as epsilon-greedy or soft-max) 
	virtual double update(const CState *s, const CAction *a, const CState *s_p, double r, double probability);
};

class CQLearning : public CSimion, public CQLearningCritic
{
protected:
	CHILD_OBJECT_FACTORY<CQPolicy> m_pQPolicy;

public:
	CQLearning(CConfigNode* pParameters);
	virtual ~CQLearning();

	virtual double update(const CState *s, const CAction *a, const CState *s_p, double r, double probability);

	double selectAction(const CState *s, CAction *a);
};

////////////////////////
//Double Q-Learning
//
//Deep Reinforcement Learning with Double Q-learning
//Hado van Hasselt, Arthur Guez and David Silver

class CDoubleQLearning : public CQLearning
{
	CLinearStateActionVFA* m_pTargetQFunction;
	CFeatureList* m_pDeferredUpdates;

	INT_PARAM m_targetUpdateFreq;
	int m_numStepsSinceLastTargetUpdate;
public:
	CDoubleQLearning(CConfigNode* pParameters);
	virtual ~CDoubleQLearning();

	virtual double update(const CState *s, const CAction *a, const CState *s_p, double r, double probability);
};

////////////////////////
//SARSA
//https://webdocs.cs.ualberta.ca/~sutton/book/ebook/node77.html

class CSARSA : public CQLearning
{
	double m_nextAProbability;
	CAction *m_nextA;
	bool m_bNextActionSelected;
public:
	CSARSA(CConfigNode* pParameters);
	virtual ~CSARSA();
	double selectAction(const CState *s, CAction *a);
	double update(const CState *s, const CAction *a, const CState *s_p, double r, double probability);
};