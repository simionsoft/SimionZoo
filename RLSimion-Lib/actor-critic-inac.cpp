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

CIncrementalNaturalActorCritic::CIncrementalNaturalActorCritic(CConfigNode* pConfigNode)
{
	m_td = 0.0;

	//critic's stuff
	m_pVFunction = CHILD_OBJECT<CLinearStateVFA>(pConfigNode, "VFunction", "The Value-function");
	m_s_features = new CFeatureList("Critic/s");
	m_s_p_features = new CFeatureList("Critic/s_p");
	m_pAlphaV = CHILD_OBJECT_FACTORY <CNumericValue>(pConfigNode, "Alpha-v", "Learning gain used by the critic");
	m_pAlphaR = CHILD_OBJECT_FACTORY <CNumericValue>(pConfigNode, "Alpha-r", "Learning gain used to average the reward");

	m_e_v = CHILD_OBJECT<CETraces>(pConfigNode, "V-ETraces", "Traces used by the critic", true);
	m_e_v->setName("Critic/e_v");

	//actor's stuff
	m_policies = MULTI_VALUE_FACTORY<CPolicy>(pConfigNode, "Policy", "The policy");

	m_w = new CFeatureList*[m_policies.size()];
	for (unsigned int i = 0; i < m_policies.size(); i++)
	{
		m_w[i] = new CFeatureList("INAC/Actor/w");
	}


	m_grad_u = new CFeatureList("INAC/Actor/grad-u");

	m_pAlphaU = CHILD_OBJECT_FACTORY<CNumericValue>(pConfigNode, "Alpha-u", "Learning gain used by the actor");

	m_e_u = CHILD_OBJECT<CETraces>(pConfigNode, "U-ETraces", "Traces used by the actor", true);
	m_e_u->setName("Actor/E-Traces");
}

CIncrementalNaturalActorCritic::~CIncrementalNaturalActorCritic()
{
	delete m_s_features;
	delete m_s_p_features;



	for (unsigned int i = 0; i < m_policies.size(); i++)
	{
		delete m_w[i];
	}
	delete[]m_w;

	delete m_grad_u;
}

void CIncrementalNaturalActorCritic::updateValue(const CState *s, const CAction *a, const CState *s_p, double r)
{
	if (CSimionApp::get()->pExperiment->isFirstStep())
		m_avg_r = 0.0;
	// Incremental Natural Actor - Critic(INAC)
	//Critic update:
	//td= r - avg_r + gamma*V(s_p) - V(s)
	//avg_r= avg_r + alpha_r * td
	//e_v= gamma* lambda*e_v + phi(s)
	//v = v + alpha_v*td*e_v
	double alpha_v = m_pAlphaV->get();
	double gamma = CSimionApp::get()->pSimGod->getGamma();
	m_pVFunction->getFeatures(s, m_s_features);
	m_pVFunction->getFeatures(s_p, m_s_p_features);
	//1. td= r - avg_r + gamma*V(s_p) - V(s)
	m_td = r - m_avg_r + gamma * m_pVFunction->get(m_s_p_features)
		- m_pVFunction->get(m_s_features);

	//2. avg_r= avg_r + alpha_r * td
	m_avg_r += m_td * m_pAlphaR->get();

	//3. e_v= gamma* lambda*e_v + phi(s)
	m_e_v->update(gamma);
	m_e_v->addFeatureList(m_s_features);
	//4. v = v + alpha_v*td*e_v
	m_pVFunction->add(m_e_v.ptr(), alpha_v*m_td);
}


void CIncrementalNaturalActorCritic::updatePolicy(const CState* s, const CState* a, const CState *s_p, double r)
{
	//Incremental Natural Actor-Critic (INAC)
	//Actor update:
	//e_u= gamma*lambda*e_u + Grad_u pi(a|s)/pi(a|s)
	//w= w - alpha_u * Grad_u pi(a|s)/pi(a|s) * Grad_u pi(a|s)/pi(a|s)^T * w + alpha_v*td*e_u
	//u= u + alpha_u * w

	//If we use Gaussian Noise added to the VFA's output:
	//Grad_u pi(a|s)/pi(a|s) = (a - pi(s)) * phi(s) / sigma*2

	//The compatible features of a gaussian policy, according to
	//http://www0.cs.ucl.ac.uk/staff/d.silver/web/Teaching_files/pg.pdf
	//is:
	// \Psi_sa= \delta log pi(s,a)= (a_t-\pi(s_t)) * phi(s_t) / sigma^2


	double gamma = CSimionApp::get()->pSimGod->getGamma();
	double alpha_v = m_pAlphaV->get();
	double alpha_u = m_pAlphaU->get();

	for (unsigned int i = 0; i < m_policies.size(); i++)
	{
		if (CSimionApp::get()->pExperiment->isFirstStep())
			m_w[i]->clear();

		m_policies[i]->getNaturalGradient(s, a, m_grad_u);
		m_grad_u->normalize();

		//1. e_u= gamma*lambda*e_u + Grad_u pi(a|s)/pi(a|s)
		m_e_u->update(gamma);
		m_e_u->addFeatureList(m_grad_u);
		//2. w= w - alpha_v * Grad_u pi(a|s)/pi(a|s) * Grad_u pi(a|s)/pi(a|s)^T * w + alpha_v*td*e_u
		double innerprod = m_grad_u->innerProduct(m_w[i]); //Grad_u pi(a|s)/pi(a|s)^T * w
		m_grad_u->mult(alpha_v*innerprod*-1.0);
		m_grad_u->applyThreshold(0.0001);
		m_w[i]->addFeatureList(m_grad_u);
		m_w[i]->addFeatureList(m_e_u.ptr(), alpha_v*m_td);
//		m_w[i]->applyThreshold(0.0001);
		//3. u= u + alpha_u * w
		m_policies[i]->addFeatures(m_w[i], alpha_u);
	}
}

void CIncrementalNaturalActorCritic::update(const CState *s, const CAction *a, const CState *s_p, double r, double behaviorProb)
{
	updateValue(s, a, s_p, r);
	updatePolicy(s, a, s_p, r);
}

double CIncrementalNaturalActorCritic::selectAction(const CState *s, CAction *a)
{
	double prob = 1.0;
	for (unsigned int i = 0; i < m_policies.size(); i++)
	{
		prob*= m_policies[i]->selectAction(s, a);
	}
	return prob;
}