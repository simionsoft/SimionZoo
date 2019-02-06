#pragma once


#include "simion.h"
#include "critic.h"

class LinearStateActionVFA;
class QPolicy;
class QFunctionUpdater;
class ETraces;
class NumericValue;
class FeatureList;
class Noise;

#include "parameters.h"

////////////////////////////////////////////////////
//Q-function based policies: abstract/factory class
class QPolicy
{
public:
	virtual ~QPolicy(){}
	static std::shared_ptr<QPolicy> getInstance(ConfigNode* pParameters);

	virtual double selectAction(LinearStateActionVFA* pQFunction, const State* s, Action* a)= 0;
};
//Epsilon-greedy policy
class QEGreedyPolicy : public QPolicy
{
	CHILD_OBJECT_FACTORY<NumericValue> m_pEpsilon;
public:
	QEGreedyPolicy(ConfigNode* pParameters);
	virtual ~QEGreedyPolicy();

	double selectAction(LinearStateActionVFA* pQFunction, const State* s, Action* a);
};
//Soft-max policy
//https://webdocs.cs.ualberta.ca/~sutton/book/2/node4.html

class QSoftMaxPolicy : public QPolicy
{
	double *m_pProbabilities;
	CHILD_OBJECT_FACTORY<NumericValue> m_pTau;
public:
	QSoftMaxPolicy(ConfigNode* pParameters);
	virtual ~QSoftMaxPolicy();
	double selectAction(LinearStateActionVFA* pQFunction, const State* s, Action* a);
};

class GreedyQPlusNoisePolicy : public QPolicy
{
	MULTI_VALUE_FACTORY<Noise> m_noise;
public:
	GreedyQPlusNoisePolicy(ConfigNode* pParameters);
	virtual ~GreedyQPlusNoisePolicy();
	double selectAction(LinearStateActionVFA* pQFunction, const State* s, Action* a);
};

/////////////////////////////
//Q-Learning
//https://webdocs.cs.ualberta.ca/~sutton/book/ebook/node78.html

class QLearningCritic : public ICritic
{
protected:
	//if this variable is true: td= r + gamma*max a' Q(s_p,a') - max a' Q(s,a')
	//else: td= r + gamma* max a' Q(s_p,a') - Q(s,a)
	bool m_bUseVFunctionAsBaseline = true;

	CHILD_OBJECT<LinearStateActionVFA> m_pQFunction;
	CHILD_OBJECT_FACTORY<NumericValue> m_pAlpha;
	FeatureList *m_pAux;
	CHILD_OBJECT<ETraces> m_eTraces;
public:
	QLearningCritic(ConfigNode* pParameters);
	virtual ~QLearningCritic();
	//the Q-Function is updated in this method (and thus, any policy derived from it such as epsilon-greedy or soft-max) 
	virtual double update(const State *s, const Action *a, const State *s_p, double r, double probability);
};

class QLearning : public Simion, public QLearningCritic
{
protected:
	CHILD_OBJECT_FACTORY<QPolicy> m_pQPolicy;

public:
	QLearning(ConfigNode* pParameters);
	virtual ~QLearning();

	virtual double update(const State *s, const Action *a, const State *s_p, double r, double probability);

	double selectAction(const State *s, Action *a);
};

////////////////////////
//Double Q-Learning
//
//Deep Reinforcement Learning with Double Q-learning
//Hado van Hasselt, Arthur Guez and David Silver

class DoubleQLearning : public QLearning
{
	LinearStateActionVFA *m_pQFunction2;

public:
	DoubleQLearning(ConfigNode* pParameters);
	virtual ~DoubleQLearning();

	virtual double update(const State *s, const Action *a, const State *s_p, double r, double probability);
};

////////////////////////
//SARSA
//https://webdocs.cs.ualberta.ca/~sutton/book/ebook/node77.html

class SARSA : public QLearning
{
	double m_nextAProbability;
	Action *m_nextA;
	bool m_bNextActionSelected;
public:
	SARSA(ConfigNode* pParameters);
	virtual ~SARSA();
	double selectAction(const State *s, Action *a);
	double update(const State *s, const Action *a, const State *s_p, double r, double probability);
};