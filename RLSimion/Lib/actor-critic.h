#pragma once
#include <vector>
#include "simion.h"
#include "critic.h"
#include "parameters.h"

class ConfigNode;
class Actor;
class VLearnerCritic;
class Policy;
class DeterministicPolicy;
class StochasticPolicy;
class ETraces;
class NumericValue;
class FeatureList;
class LinearStateVFA;
class LinearStateActionVFA;

class ActorCritic: public Simion
{
	double m_td;
	CHILD_OBJECT<Actor> m_pActor;
	CHILD_OBJECT_FACTORY<ICritic> m_pCritic;
public:
	ActorCritic(ConfigNode* pParameters);
	virtual ~ActorCritic() = default;

	virtual double selectAction(const State *s, Action *a);

	virtual double update(const State *s, const Action *a, const State *s_p, double r, double behaviorProb);
};

class IncrementalNaturalActorCritic : public Simion
{
	//"Model-free Reinforcement Learning with Continuous Action in Practice"
	//Thomas Degris, Patrick M. Pilarski, Richard S. Sutton
	//2012 American Control Conference

	//td error
	double m_td;

	double m_v_s, m_v_s_p;

	//linear state value function (represented by v^t * x in the paper)
	CHILD_OBJECT<LinearStateVFA> m_pVFunction;

	//list of policies
	MULTI_VALUE_FACTORY<Policy> m_policies;

	//average reward
	double m_avg_r;

	//eligibility traces
	ETraces** m_e_u;
	CHILD_OBJECT<ETraces> m_e_v;

	//gradient of the policy with respect to its parameters
	FeatureList *m_grad_u;

	//buffer to store features of the value function activated by the state s and the state s'
	FeatureList *m_s_features;
	FeatureList *m_s_p_features;

	//list of weight vector w for updating the value of v
	FeatureList **m_w;

	//learning rates
	CHILD_OBJECT_FACTORY<NumericValue> m_pAlphaU;
	CHILD_OBJECT_FACTORY<NumericValue> m_pAlphaV;
	CHILD_OBJECT_FACTORY<NumericValue> m_pAlphaR;

	//updates the policy/the actor
	void updatePolicy(const State *s, const Action *a, const State *s_p, double r);

	//updates the value/the critic
	void updateValue(const State *s, const Action *a, const State *s_p, double r);
public:
	IncrementalNaturalActorCritic(ConfigNode *pParameters);
	virtual ~IncrementalNaturalActorCritic();

	//selects an according to the learned policy pi(a|s)
	virtual double selectAction(const State *s, Action *a);

	//updates the critic and the actor
	virtual double update(const State *s, const Action *a, const State *s_p, double r, double behaviorProb);
};

class OffPolicyActorCritic : public Simion
{
	//"Off-Policy Actor-Critic"
	//Thomas Degris, Martha White, Richard S. Sutton
	//Proceedings of the 29 th International Conference on Machine Learning, Edinburgh, Scotland, UK, 2012
	//arXiv:1205.4839v5 [cs.LG] 20 Jun 2013

	//td error
	double m_td;

	//pi(a|s)/b(a|s) while pi(a|s) is the learned policy and b(s|a) is arbitrary chosen
	double m_rho;

	//linear state value function (represented by v^T * x(s) in the paper)
	CHILD_OBJECT<LinearStateVFA> m_pVFunction;

	//list of policies
	MULTI_VALUE_FACTORY<Policy> m_policies;

	//average reward
	double m_avg_r;

	//eligibility traces
	ETraces** m_e_u;
	CHILD_OBJECT<ETraces> m_e_v;

	//gradient of the policy with respect to its parameters
	FeatureList *m_grad_u;

	//buffer to store features of the value function activated by the state s and the state s'
	FeatureList *m_s_features;
	FeatureList *m_s_p_features;

	//list of weight vector w for updating the value of v
	FeatureList **m_w;

	//learning rates
	CHILD_OBJECT_FACTORY<NumericValue> m_pAlphaU;
	CHILD_OBJECT_FACTORY<NumericValue> m_pAlphaV;
	CHILD_OBJECT_FACTORY<NumericValue> m_pAlphaW;


	//updates the policy/the actor
	void updatePolicy(const State *s, const Action *a, const State *s_p, double r);

	//updates the value/the critic
	void updateValue(const State *s, const Action *a, const State *s_p, double r);

public:
	OffPolicyActorCritic(ConfigNode *pParameters);
	virtual ~OffPolicyActorCritic();

	//selects an according to the learned policy pi(a|s)
	virtual double selectAction(const State *s, Action *a);

	//updates the critic and the actor
	virtual double update(const State *s, const Action *a, const State *s_p, double r, double behaviorProb);
};


class OffPolicyDeterministicActorCritic : public Simion
{
	//"off-policy deterministic actorcritic (OPDAC)" in "Deterministic Policy Gradient Algorithms"
	//David Silver, Guy Lever, Nicolas Heess, Thomas Degris, Daan Wierstra, Martin Riedmiller 
	//Proceedings of the 31 st International Conference on Machine Learning, Beijing, China, 2014. JMLR: W&CP volume 32

	//td error
	double m_td;

	//buffer to app
	Action* a_p;

	//list base policies beta_i(a|s) for each pi_i(a|s)
	MULTI_VALUE_FACTORY<Policy> m_beta_policies;

	//linear state action value function
	//(in the paper this is a general function without any more knowledge about it)
	CHILD_OBJECT<LinearStateActionVFA> m_pQFunction;

	//list of policies
	MULTI_VALUE_FACTORY<DeterministicPolicy> m_policies;

	//gradient of the policy and the value function with respect to its parameters
	FeatureList *m_grad_Q;
	FeatureList *m_grad_mu;

	//buffer to store features of the value function activated by the state s and the state s'
	FeatureList *m_s_features;
	FeatureList *m_s_p_features;

	//list of weight vector w for updating the value of Q(s, a)
	FeatureList **m_w;

	//learning rates
	CHILD_OBJECT_FACTORY<NumericValue> m_pAlphaTheta;
	CHILD_OBJECT_FACTORY<NumericValue> m_pAlphaW;

	//updates the policy/the actor
	void updatePolicy(const State *s, const Action *a, const State *s_p, double r);

	//updates the value/the critic
	void updateValue(const State *s, const Action *a, const State *s_p, double r);
public:
	OffPolicyDeterministicActorCritic(ConfigNode *pParameters);
	virtual ~OffPolicyDeterministicActorCritic();

	//selects an according to the learned policy pi(a|s)
	virtual double selectAction(const State *s, Action *a);

	//updates the critic and the actor
	virtual double update(const State *s, const Action *a, const State *s_p, double r, double behaviorProb);
};