#pragma once
#include <vector>
#include "simion.h"
#include "critic.h"
#include "parameters.h"

class CConfigNode;
class CActor;
class CVLearnerCritic;
class CPolicy;
class CDeterministicPolicy;
class CStochasticPolicy;
class CETraces;
class CNumericValue;
class CFeatureList;
class CLinearStateVFA;
class CLinearStateActionVFA;

class CActorCritic: public CSimion
{
	double m_td;
	CHILD_OBJECT<CActor> m_pActor;
	CHILD_OBJECT_FACTORY<ICritic> m_pCritic;
public:
	CActorCritic(CConfigNode* pParameters);
	virtual ~CActorCritic() = default;

	virtual double selectAction(const CState *s, CAction *a);

	virtual double update(const CState *s, const CAction *a, const CState *s_p, double r, double behaviorProb);
};

class CIncrementalNaturalActorCritic : public CSimion
{
	//"Model-free Reinforcement Learning with Continuous Action in Practice"
	//Thomas Degris, Patrick M. Pilarski, Richard S. Sutton
	//2012 American Control Conference

	//td error
	double m_td;

	double m_v_s, m_v_s_p;

	//linear state value function (represented by v^t * x in the paper)
	CHILD_OBJECT<CLinearStateVFA> m_pVFunction;

	//list of policies
	MULTI_VALUE_FACTORY<CPolicy> m_policies;

	//average reward
	double m_avg_r;

	//eligibility traces
	CETraces** m_e_u;
	CHILD_OBJECT<CETraces> m_e_v;

	//gradient of the policy with respect to its parameters
	CFeatureList *m_grad_u;

	//buffer to store features of the value function activated by the state s and the state s'
	CFeatureList *m_s_features;
	CFeatureList *m_s_p_features;

	//list of weight vector w for updating the value of v
	CFeatureList **m_w;

	//learning rates
	CHILD_OBJECT_FACTORY<CNumericValue> m_pAlphaU;
	CHILD_OBJECT_FACTORY<CNumericValue> m_pAlphaV;
	CHILD_OBJECT_FACTORY<CNumericValue> m_pAlphaR;

	//updates the policy/the actor
	void updatePolicy(const CState *s, const CAction *a, const CState *s_p, double r);

	//updates the value/the critic
	void updateValue(const CState *s, const CAction *a, const CState *s_p, double r);
public:
	CIncrementalNaturalActorCritic(CConfigNode *pParameters);
	virtual ~CIncrementalNaturalActorCritic();

	//selects an according to the learned policy pi(a|s)
	virtual double selectAction(const CState *s, CAction *a);

	//updates the critic and the actor
	virtual double update(const CState *s, const CAction *a, const CState *s_p, double r, double behaviorProb);
};

class COffPolicyActorCritic : public CSimion
{
	//"Off-Policy Actor-Critic"
	//Thomas Degris, Martha White, Richard S. Sutton
	//Proceedings of the 29 th International Conference on Machine Learning, Edinburgh, Scotland, UK, 2012
	//arXiv:1205.4839v5 [cs.LG] 20 Jun 2013

	//td error
	double m_td;

	//pi(a|s)/b(a|s) while pi(a|s) is the learned policy and b(s|a) is arbitrary chosen
	double m_rho;

	//list base policies b_i(a|s) for each pi_i(a|s)
	MULTI_VALUE_FACTORY<CPolicy> m_b_policies;

	//linear state value function (represented by v^T * x(s) in the paper)
	CHILD_OBJECT<CLinearStateVFA> m_pVFunction;

	//list of policies
	MULTI_VALUE_FACTORY<CPolicy> m_policies;

	//average reward
	double m_avg_r;

	//eligibility traces
	CETraces** m_e_u;
	CHILD_OBJECT<CETraces> m_e_v;

	//gradient of the policy with respect to its parameters
	CFeatureList *m_grad_u;

	//buffer to store features of the value function activated by the state s and the state s'
	CFeatureList *m_s_features;
	CFeatureList *m_s_p_features;

	//list of weight vector w for updating the value of v
	CFeatureList **m_w;

	//learning rates
	CHILD_OBJECT_FACTORY<CNumericValue> m_pAlphaU;
	CHILD_OBJECT_FACTORY<CNumericValue> m_pAlphaV;
	CHILD_OBJECT_FACTORY<CNumericValue> m_pAlphaW;


	//updates the policy/the actor
	void updatePolicy(const CState *s, const CAction *a, const CState *s_p, double r);

	//updates the value/the critic
	void updateValue(const CState *s, const CAction *a, const CState *s_p, double r);

public:
	COffPolicyActorCritic(CConfigNode *pParameters);
	virtual ~COffPolicyActorCritic();

	//selects an according to the learned policy pi(a|s)
	virtual double selectAction(const CState *s, CAction *a);

	//updates the critic and the actor
	virtual double update(const CState *s, const CAction *a, const CState *s_p, double r, double behaviorProb);
};


class COffPolicyDeterministicActorCritic : public CSimion
{
	//"off-policy deterministic actorcritic (OPDAC)" in "Deterministic Policy Gradient Algorithms"
	//David Silver, Guy Lever, Nicolas Heess, Thomas Degris, Daan Wierstra, Martin Riedmiller 
	//Proceedings of the 31 st International Conference on Machine Learning, Beijing, China, 2014. JMLR: W&CP volume 32

	//td error
	double m_td;

	//buffer to app
	CAction* a_p;

	//list base policies beta_i(a|s) for each pi_i(a|s)
	MULTI_VALUE_FACTORY<CPolicy> m_beta_policies;

	//linear state action value function
	//(in the paper this is a general function without any more knowledge about it)
	CHILD_OBJECT<CLinearStateActionVFA> m_pQFunction;

	//list of policies
	MULTI_VALUE_FACTORY<CDeterministicPolicy> m_policies;

	//gradient of the policy and the value function with respect to its parameters
	CFeatureList *m_grad_Q;
	CFeatureList *m_grad_mu;

	//buffer to store features of the value function activated by the state s and the state s'
	CFeatureList *m_s_features;
	CFeatureList *m_s_p_features;

	//list of weight vector w for updating the value of Q(s, a)
	CFeatureList **m_w;

	//learning rates
	CHILD_OBJECT_FACTORY<CNumericValue> m_pAlphaTheta;
	CHILD_OBJECT_FACTORY<CNumericValue> m_pAlphaW;

	//updates the policy/the actor
	void updatePolicy(const CState *s, const CAction *a, const CState *s_p, double r);

	//updates the value/the critic
	void updateValue(const CState *s, const CAction *a, const CState *s_p, double r);
public:
	COffPolicyDeterministicActorCritic(CConfigNode *pParameters);
	virtual ~COffPolicyDeterministicActorCritic();

	//selects an according to the learned policy pi(a|s)
	virtual double selectAction(const CState *s, CAction *a);

	//updates the critic and the actor
	virtual double update(const CState *s, const CAction *a, const CState *s_p, double r, double behaviorProb);
};
