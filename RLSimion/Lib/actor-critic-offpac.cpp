/*
	SimionZoo: A framework for online model-free Reinforcement Learning on continuous
	control problems

	Copyright (c) 2016 SimionSoft. https://github.com/simionsoft

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#include "features.h"
#include "etraces.h"
#include "vfa.h"
#include "actor-critic.h"
#include "noise.h"
#include "../Common/named-var-set.h"
#include "vfa-critic.h"
#include "config.h"
#include "experiment.h"
#include "parameters-numeric.h"
#include "policy.h"
#include "app.h"
#include "simgod.h"
#include "logger.h"

OffPolicyActorCritic::OffPolicyActorCritic(ConfigNode* pConfigNode)
{
	SimionApp::get()->pLogger->addVarToStats("TD-error", "TD-error", m_td);
	SimionApp::get()->pLogger->addVarToStats("m_rho", "m_rho", m_rho);
	//td error
	m_td = 0.0;

	//pi(a|s)/b(a|s)
	m_rho = 0.0;

	/*critic's stuff*/
	//linear state value function (represented by v^t * x in the paper)
	m_pVFunction = CHILD_OBJECT<LinearStateVFA>(pConfigNode, "VFunction", "The Value-function");
	SimionApp::get()->registerStateActionFunction("V", m_pVFunction.ptr());

	//buffer to store features of the value function activated by the state s and the state s'
	m_s_features = new FeatureList("Critic/s");
	m_s_p_features = new FeatureList("Critic/s_p");
	//learning rates
	m_pAlphaV = CHILD_OBJECT_FACTORY <NumericValue>(pConfigNode, "Alpha-v", "Learning gain used by the critic");
	m_pAlphaW = CHILD_OBJECT_FACTORY <NumericValue>(pConfigNode, "Alpha-w", "Learning gain used to average the reward");
	//eligibility trace
	m_e_v = CHILD_OBJECT<ETraces>(pConfigNode, "ETraces", "Traces used by the critic and the actor", true);
	m_e_v->setName("Critic/e_v");


	/*actor's stuff*/
	//list of policies
	m_policies = MULTI_VALUE_FACTORY<Policy>(pConfigNode, "Policy", "The policy");

	//list of weight vector w for updating the value of v
	m_w = new FeatureList*[m_policies.size()];
	for (unsigned int i = 0; i < m_policies.size(); i++)
	{
		m_w[i] = new FeatureList("INAC/Actor/w");
	}

	//gradient of the policy with respect to its parameters
	m_grad_u = new FeatureList("INAC/Actor/grad-u");

	//learning rate
	m_pAlphaU = CHILD_OBJECT_FACTORY<NumericValue>(pConfigNode, "Alpha-u", "Learning gain used by the actor");
	//eligibility trace
	m_e_u = new ETraces*[m_policies.size()];
	for (unsigned int i = 0; i < m_policies.size(); i++)
	{
		m_e_u[i] = new ETraces(pConfigNode);
		//we name all the same to avoid having to store all pointers and then freeing them in the constructor
		//the actual dimension it belongs to doesn't seem very important at the moment
		static std::string name= "Actor/E-Trace";
		m_e_u[i]->setName(name.c_str());

		//copy the settings of the e_v trace
		//(we do this, because we just want one entry in the settings for all traces
		//and not for each trace an individual entry)
		m_e_u[i]->setLambda(m_e_v->getLambda());
		m_e_u[i]->setTreshold(m_e_v->getTreshold());
		m_e_u[i]->setReplace(m_e_v->getReplace());
	}
}

OffPolicyActorCritic::~OffPolicyActorCritic()
{
	delete m_s_features;
	delete m_s_p_features;

	for (unsigned int i = 0; i < m_policies.size(); i++)
	{
		delete m_w[i];
		delete m_e_u[i];
	}
	delete[] m_w;
	delete[] m_e_u;

	delete m_grad_u;
}

#include <iostream>

void OffPolicyActorCritic::updateValue(const State *s, const Action *a, const State *s_p, double r)
{
	//td = r + gamma*V(s') - V(s)
	//rho = pi(a|s)/b(a|s)

	//update the value/critic:
	//(is ONLY holds for LINEAR VFA, V(s) = w^T * x(s))
	//e_v = e_v + rho * (x(s) + gamma(s) * lambda * e_v)
	//v   = v + alpha_v * (td * e_v - gamma(s')*(1-lambda)*(w^T*e_v)*x(s))
	//w   = w + alpha_w * (td * e_v - (w^T*x(s))*x(s))

	double gamma = SimionApp::get()->pSimGod->getGamma();
	double alpha_v = m_pAlphaV->get();
	double alpha_w = m_pAlphaW->get();

	m_pVFunction->getFeatures(s, m_s_features);
	m_pVFunction->getFeatures(s_p, m_s_p_features);

	//td = r + gamma*V(s') - V(s)
	m_td = r + gamma * m_pVFunction->get(s_p) - m_pVFunction->get(s);

	//rho = pi(a|s)/b(a|s)
	m_rho = 1.0;

	//v   = v + alpha_v * (td * e_v - gamma(s')*(1-lambda)*(w^T*e_v)*x(s))
	//w   = w + alpha_w * (td * e_v - (w^T*x(s))*x(s))
	m_e_v->update(gamma);
	m_e_v->addFeatureList(m_s_features, 1.0);
//	m_e_v->mult(m_rho);
	m_e_v->applyThreshold(m_e_v->getTreshold());

	for (unsigned int i = 0; i < m_policies.size(); i++)
	{
		if (SimionApp::get()->pExperiment->isFirstStep())
			m_w[i]->clear();

		m_pVFunction->add(m_e_v.ptr(), m_td*alpha_v);
		double factor = -alpha_v * gamma * (1.0 - m_e_v->getLambda()) * m_w[i]->innerProduct(m_e_v.ptr());
		m_pVFunction->add(m_s_features, factor);

		m_w[i]->addFeatureList(m_e_v.ptr(), alpha_w * m_td);
		factor = -alpha_w * m_w[i]->innerProduct(m_s_features);
		m_w[i]->addFeatureList(m_s_features, factor);
	}
}

void OffPolicyActorCritic::updatePolicy(const State* s, const State* a, const State *s_p, double r)
{
	//update the policy/critic
	//psi(s,a) = (grad_u pi(a|s)) / (pi(a|s))
	//e_u = e_u + rho * (psi(s, a) + gamma(s) * lambda * e_u)
	//u   = u + alpha_u * td * e_u

	double gamma = SimionApp::get()->pSimGod->getGamma();
	double alpha_u = m_pAlphaU->get();

	for (unsigned int i = 0; i < m_policies.size(); i++)
	{
		if (SimionApp::get()->pExperiment->isFirstStep())
			m_w[i]->clear();

		//calculate the gradient
		m_grad_u->clear();
		m_policies[i]->getParameterGradient(s, a, m_grad_u);
		m_e_u[i]->update(m_rho*gamma);
		m_e_u[i]->addFeatureList(m_grad_u, m_rho);

		m_policies[i]->addFeatures(m_e_u[i], alpha_u*m_td);
	}
}

/// <summary>
/// Updates the policy and the value function using the Incremental Natural Actor Critic algorithm in
/// "Off-Policy Actor-Critic" (Thomas Degris, Martha White, Richard S. Sutton),
/// Proceedings of the 29 th International Conference on Machine Learning, Edinburgh, Scotland, UK, 2012.
/// arXiv:1205.4839v5 [cs.LG] 20 Jun 2013
/// </summary>
/// <param name="s">Initial state</param>
/// <param name="a">Action</param>
/// <param name="s_p">Resultant state</param>
/// <param name="r">Reward</param>
/// <param name="behaviorProb">Probability by which the actor selected the action</param>
/// <returns>Should return the TD error. Currently unused</returns>
double OffPolicyActorCritic::update(const State *s, const Action *a, const State *s_p, double r, double behaviorProb)
{
	updateValue(s, a, s_p, r);
	updatePolicy(s, a, s_p, r);
	return 1.0;
}

/// <summary>
/// The actor selects an action following the policies it is learning
/// </summary>
/// <param name="s">Initial state</param>
/// <param name="a">Action</param>
/// <returns>The probability by which the action was selected</returns>
double OffPolicyActorCritic::selectAction(const State *s, Action *a)
{
	double prob = 1.0;
	for (unsigned int i = 0; i < m_policies.size(); i++)
	{
		prob *= m_policies[i]->selectAction(s, a);
	}
	return prob;
}
