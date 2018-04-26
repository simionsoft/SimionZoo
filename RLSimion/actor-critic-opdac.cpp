#include "features.h"
#include "etraces.h"
#include "vfa.h"
#include "actor-critic.h"
#include "noise.h"
#include "named-var-set.h"
#include "vfa-critic.h"
#include "config.h"
#include "experiment.h"
#include "parameters-numeric.h"
#include "policy.h"
#include "app.h"
#include "logger.h"
#include "simgod.h"

//Implementation according to
//http://proceedings.mlr.press/v32/silver14.pdf

OffPolicyDeterministicActorCritic::OffPolicyDeterministicActorCritic(ConfigNode* pConfigNode)
{

	SimionApp::get()->pLogger->addVarToStats("TD-error", "TD-error", m_td);

	//td error
	m_td = 0.0;

	//base policies beta_i(a|s)
	m_beta_policies = MULTI_VALUE_FACTORY<Policy>(pConfigNode, "beta-Policy", "The base-policy beta(a|s)");

	//critic's stuff
	//linear state action value function
	//(in the paper this is a general function without any more knowledge about it)
	m_pQFunction = CHILD_OBJECT<LinearStateActionVFA>(pConfigNode, "QFunction", "The Q-function");
	//buffer to store features of the value function activated by the state s and the state s'
	m_s_features = new FeatureList("Critic/s");
	m_s_p_features = new FeatureList("Critic/s_p");
	//learning rates
	m_pAlphaW = CHILD_OBJECT_FACTORY <NumericValue>(pConfigNode, "Alpha-w", "Learning gain used by the critic");


	//actor's stuff
	//list of policies
	m_policies = MULTI_VALUE_FACTORY<DeterministicPolicy>(pConfigNode, "Policy", "The deterministic policy");
	//gradient of the policy with respect to its parameters
	m_grad_mu = new FeatureList("OPDAC/Actor/grad-mu");
	//learning rate
	m_pAlphaTheta = CHILD_OBJECT_FACTORY<NumericValue>(pConfigNode, "Alpha-theta", "Learning gain used by the actor");

}

OffPolicyDeterministicActorCritic::~OffPolicyDeterministicActorCritic()
{
	delete m_s_features;
	delete m_s_p_features;
}

#include <iostream>
void OffPolicyDeterministicActorCritic::updateValue(const State *s, const Action *a, const State *s_p, double r)
{
	//td = r + gamma*V(s') - V(s)

	//update the value/critic:
	//w = w + alpha_w * td * grad_w(Q^w)(s_t, a_t)

	double gamma = SimionApp::get()->pSimGod->getGamma();
	double alpha_w = m_pAlphaW->get();

	//select new action a_(t+1) = mu(s_(t+1))
	for (size_t i = 0; i < m_policies.size(); i++)
	{
		m_policies[i]->selectAction(s_p, a_p);
	}

	//td = r + gamma*V(s') - V(s)
	m_td = r + gamma * m_pQFunction->get(s_p, a_p) - m_pQFunction->get(s, a);

	//w(t+1) = w(t) + alpha_w * td * grad_w(Q^w)(s(t), a(t))
	m_pQFunction->getFeatures(s, a, m_s_features);
	m_pQFunction->add(m_s_features, alpha_w*m_td);
}

void OffPolicyDeterministicActorCritic::updatePolicy(const State* s, const State* a, const State *s_p, double r)
{
	//update the policy/critic
	//theta(t+1) = theta(t) + alpha_theta * grad_theta(mu_theta(s(t)) * grad_a(Q^w(s(t), a(t))

	double alpha_theta = m_pAlphaTheta->get();

	for (unsigned int i = 0; i < m_policies.size(); i++)
	{
		if (SimionApp::get()->pExperiment->isFirstStep())
			m_w[i]->clear();

		//calculate the gradients
		m_grad_Q->clear();
		m_grad_mu->clear();

		//get the grad_theta(mu_theta(s(t))
		m_policies[i]->getParameterGradient(s, a, m_grad_mu);


		//get the grad_a(Q^w(s(t), a(t))
		//we now assume that Q^q(s,a) is compatible with the policy mu_theta(s)
		//this means: grad_a(Q^w(s(t), a(t)) = grad_theta(mu_theta(s))^T * w
		double grad_prod = m_grad_mu->innerProduct(m_grad_mu);

		//calculate the product grad_theta(mu_theta(s(t)) * grad_a(Q^w(s(t), a(t)) and store it in m_grad_mu
		
		m_policies[i]->addFeatures(m_s_features, grad_prod*alpha_theta);
	}
}

double OffPolicyDeterministicActorCritic::update(const State *s, const Action *a, const State *s_p, double r, double behaviorProb)
{
	updateValue(s, a, s_p, r);
	updatePolicy(s, a, s_p, r);
	return 1.0;
}

double OffPolicyDeterministicActorCritic::selectAction(const State *s, Action *a)
{
	double prob = 1.0;
	if (!SimionApp::get()->pExperiment->isEvaluationEpisode())
	{
		for (unsigned int i = 0; i < m_policies.size(); i++)
		{
			prob *= m_beta_policies[i]->selectAction(s, a);
		}
	}
	else
	{
		for (unsigned int i = 0; i < m_policies.size(); i++)
		{
			prob *= m_policies[i]->selectAction(s, a);
		}
	}
	return prob;
}