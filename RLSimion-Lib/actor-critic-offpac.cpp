#include "stdafx.h"
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

COffPolicyActorCritic::COffPolicyActorCritic(CConfigNode* pConfigNode)
{
	CSimionApp::get()->pLogger->addVarToStats("TD-error", "TD-error", &m_td);
	CSimionApp::get()->pLogger->addVarToStats("m_rhor", "m_rho", &m_rho);
	//td error
	m_td = 0.0;

	//pi(a|s)/b(a|s)
	m_rho = 0.0;

	//base policy b(a|s)
	m_b_policies = MULTI_VALUE_FACTORY<CPolicy>(pConfigNode, "b-Policy", "The base-policy b(a|s)");

	/*critic's stuff*/
	//linear state value function (represented by v^t * x in the paper)
	m_pVFunction = CHILD_OBJECT<CLinearStateVFA>(pConfigNode, "VFunction", "The Value-function");
	//buffer to store features of the value function activated by the state s and the state s'
	m_s_features = new CFeatureList("Critic/s");
	m_s_p_features = new CFeatureList("Critic/s_p");
	//learning rates
	m_pAlphaV = CHILD_OBJECT_FACTORY <CNumericValue>(pConfigNode, "Alpha-v", "Learning gain used by the critic");
	m_pAlphaW = CHILD_OBJECT_FACTORY <CNumericValue>(pConfigNode, "Alpha-w", "Learning gain used to average the reward");
	//lambda factor
	m_lambda = CHILD_OBJECT_FACTORY <CNumericValue>(pConfigNode, "OffPac-Lambda", "Lambda factor of the GTD(lambda) critic's update");
	//eligibility trace
	m_e_v = CHILD_OBJECT<CETraces>(pConfigNode, "V-ETraces", "Traces used by the critic", true);
	m_e_v->setName("Critic/e_v");



	/*actor's stuff*/
	//list of policies
	m_policies = MULTI_VALUE_FACTORY<CPolicy>(pConfigNode, "Policy", "The policy");

	//list of weight vector w for updating the value of v
	m_w = new CFeatureList*[m_policies.size()];
	for (unsigned int i = 0; i < m_policies.size(); i++)
	{
		m_w[i] = new CFeatureList("INAC/Actor/w");
	}

	//gradient of the policy with respect to its parameters
	m_grad_u = new CFeatureList("INAC/Actor/grad-u");

	//learning rate
	m_pAlphaU = CHILD_OBJECT_FACTORY<CNumericValue>(pConfigNode, "Alpha-u", "Learning gain used by the actor");
	//eligibility trace
	m_e_u = new CETraces*[m_policies.size()];
	for (unsigned int i = 0; i < m_policies.size(); i++)
	{
		m_e_u[i] = new CETraces(pConfigNode);
		char* buffer = new char[strlen("Actor/E-Trace %i") + 100];
		sprintf(buffer, "Actor/E-Trace %i", i);
		m_e_u[i]->setName(buffer);
	}
}

COffPolicyActorCritic::~COffPolicyActorCritic()
{
	delete m_s_features;
	delete m_s_p_features;

	for (unsigned int i = 0; i < m_policies.size(); i++)
	{
		delete m_w[i];
	}
	delete[] m_w;

	delete m_grad_u;
}

#include <iostream>
void COffPolicyActorCritic::updateValue(const CState *s, const CAction *a, const CState *s_p, double r)
{
	//td = r + gamma*V(s') - V(s)
	//rho = pi(a|s)/b(a|s)

	//update the value/critic:
	//(is ONLY holds for LINEAR VFA, V(s) = w^T * x(s))
	//e_v = e_v + rho * (x(s) + gamma(s) * lambda * e_v)
	//v   = v + alpha_v * (td * e_v - gamma(s')*(1-lambda)*(w^T*e_v)*x(s))
	//w   = w + alpha_w * (td * e_v - (w^T*x(s))*x(s))

	double gamma = CSimionApp::get()->pSimGod->getGamma();
	double lambda = m_lambda->get();
	double alpha_v = m_pAlphaV->get();
	double alpha_w = m_pAlphaW->get();

	m_pVFunction->getFeatures(s, m_s_features);
	m_pVFunction->getFeatures(s_p, m_s_p_features);

	//td = r + gamma*V(s') - V(s)
	m_td = r + gamma * m_pVFunction->get(s_p) - m_pVFunction->get(s);

	//rho = pi(a|s)/b(a|s)
	m_rho = 1.0;
	for (unsigned int i = 0; i < m_policies.size(); i++)
	{
		double b_prob = m_b_policies[i]->getProbability(s, a, true);
		//bStochastic must be true, otherwise this will always yield 1.0 which would be fatal
		double prob = m_policies[i]->getProbability(s, a, true);
		m_rho *= prob / b_prob;
	}

	//v   = v + alpha_v * (td * e_v - gamma(s')*(1-lambda)*(w^T*e_v)*x(s))
	//w   = w + alpha_w * (td * e_v - (w^T*x(s))*x(s))
	m_e_v->update(gamma);
	m_e_v->addFeatureList(m_s_features, 1.0);
	m_e_v->mult(m_rho);

	for (unsigned int i = 0; i < m_policies.size(); i++)
	{
		m_pVFunction->add(m_e_v.ptr(), m_td*alpha_v);
		double factor = -alpha_v * gamma * (1.0 - m_e_v->getLambda()) * m_w[i]->innerProduct(m_e_v.ptr());
		m_pVFunction->add(m_s_features, factor);
		
		m_w[i]->addFeatureList(m_e_v.ptr(), alpha_w * m_td);
		factor = -alpha_w * m_w[i]->innerProduct(m_s_features);
		m_w[i]->addFeatureList(m_s_features, factor);
	}
}

void COffPolicyActorCritic::updatePolicy(const CState* s, const CState* a, const CState *s_p, double r)
{
	//update the policy/critic
	//psi(s,a) = (grad_u pi(a|s)) / (pi(a|s))
	//e_u = e_u + rho * (psi(s, a) + gamma(s) * lambda * e_u)
	//u   = u + alpha_u * td * e_u

	double gamma = CSimionApp::get()->pSimGod->getGamma();
	double lambda = m_lambda->get();
	double alpha_u = m_pAlphaU->get();

	for (unsigned int i = 0; i < m_policies.size(); i++)
	{
		if (CSimionApp::get()->pExperiment->isFirstStep())
			m_w[i]->clear();

		//calculate the gradient
		m_grad_u->clear();
		m_policies[i]->getNaturalGradient(s, a, m_grad_u);
		m_e_u[i]->addFeatureList(m_grad_u, m_rho);
		m_e_u[i]->addFeatureList(m_e_u[i], m_rho*gamma*m_e_u[i]->getLambda());

		m_policies[i]->addFeatures(m_e_u[i], alpha_u*m_td);
	}
}

double COffPolicyActorCritic::update(const CState *s, const CAction *a, const CState *s_p, double r, double behaviorProb)
{
	updateValue(s, a, s_p, r);
	updatePolicy(s, a, s_p, r);
	return 1.0;
}

double COffPolicyActorCritic::selectAction(const CState *s, CAction *a)
{
	double prob = 1.0;
	if (!CSimionApp::get()->pExperiment->isEvaluationEpisode())
	{
		for (unsigned int i = 0; i < m_policies.size(); i++)
		{
			prob *= m_b_policies[i]->selectAction(s, a);
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