#include "stdafx.h"

#include "features.h"
#include "etraces.h"
#include "vfa.h"
#include "actor-critic.h"
#include "noise.h"
#include "named-var-set.h"
#include "vfa-critic.h"
#include "parameters.h"
#include "globals.h"
#include "experiment.h"
#include "parameters-numeric.h"
#include "policy.h"
#include "app.h"

CLASS_CONSTRUCTOR(CIncrementalNaturalActorCritic)
{
	m_td = 0.0;


	//critic's stuff
	CHILD_CLASS(m_pVFunction, "VFunction", "The Value-function", false, CLinearStateVFA);
	m_s_features = new CFeatureList("Critic/s");
	m_s_p_features = new CFeatureList("Critic/s_p");
	NUMERIC_VALUE(m_pAlphaV, "Alpha-v","Learning gain used by the critic");
	NUMERIC_VALUE(m_pAlphaR,"Alpha-r","Learning gain used to average the reward");
	NUMERIC_VALUE(m_pGamma, "Gamma","Gamma parameter of the MDP");
	CHILD_CLASS(m_e_v,"V-ETraces","Traces used by the critic",true,CETraces,"Critic/e_v");

	//actor's stuff
	m_numPolicies = pParameters->countChildren("Policy");
	if (!m_numPolicies) throw std::exception("No policy defined for the INAC Simion");
	CParameters* pChild = pParameters->getChild("Policy");
	m_pPolicies = new CPolicy*[m_numPolicies];
	m_w = new CFeatureList*[m_numPolicies];
	for (int i = 0; i < m_numPolicies; i++)
	{
		MULTI_VALUED_FACTORY(m_pPolicies[i], "Policy", "The Policy", CPolicy, pChild);

		//there's something 
		m_w[i] = new CFeatureList("INAC-w",false,true);
		pChild = pChild->getNextChild("Policy");
	}

	m_grad_u = new CFeatureList("Actor/grad-u");
	NUMERIC_VALUE(m_pAlphaU, "Alpha-u","Learning gain used by the actor");
	CHILD_CLASS(m_e_u, "U-ETraces","Traces used by the actor",true, CETraces, "Actor/E-Traces");
	END_CLASS();
}

CIncrementalNaturalActorCritic::~CIncrementalNaturalActorCritic()
{

	delete m_pVFunction;
	delete m_s_features;
	delete m_s_p_features;

	delete m_pAlphaV;
	delete m_pAlphaR;
	delete m_pGamma;

	delete m_e_v;


	for (int i = 0; i < m_numPolicies; i++)
	{
		delete m_pPolicies[i];
		delete m_w[i];
	}
	delete[] m_pPolicies;
	delete[]m_w;

	delete m_grad_u;
	delete m_pAlphaU;
	delete m_e_u;
}

void CIncrementalNaturalActorCritic::updateValue(const CState *s, const CAction *a, const CState *s_p, double r)
{
	if (CApp::get()->Experiment.isFirstStep())
		m_avg_r = 0.0;
	// Incremental Natural Actor - Critic(INAC)
	//Critic update:
	//td= r - avg_r + gamma*V(s_p) - V(s)
	//avg_r= avg_r + alpha_r * td
	//e_v= gamma* lambda*e_v + phi(s)
	//v = v + alpha_v*td*e_v
	double alpha_v = m_pAlphaV->getValue();
	double gamma = m_pGamma->getValue();
	m_pVFunction->getFeatures(s, m_s_features);
	m_pVFunction->getFeatures(s_p, m_s_p_features);
	//1. td= r - avg_r + gamma*V(s_p) - V(s)
	m_td = r - m_avg_r + gamma * m_pVFunction->getValue(m_s_p_features)
		- m_pVFunction->getValue(m_s_features);

	//2. avg_r= avg_r + alpha_r * td
	m_avg_r += m_td * m_pAlphaR->getValue();

	//3. e_v= gamma* lambda*e_v + phi(s)
	m_e_v->update(gamma);
	m_e_v->addFeatureList(m_s_features);
	//4. v = v + alpha_v*td*e_v
	m_pVFunction->add(m_e_v, alpha_v*m_td);
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


	double gamma = m_pGamma->getValue();
	double alpha_v = m_pAlphaV->getValue();
	double alpha_u = m_pAlphaU->getValue();

	for (int i = 0; i < m_numPolicies; i++)
	{
		if (CApp::get()->Experiment.isFirstStep())
			m_w[i]->clear();

		m_pPolicies[i]->getNaturalGradient(s, a, m_grad_u);
		m_grad_u->normalize();

		//1. e_u= gamma*lambda*e_u + Grad_u pi(a|s)/pi(a|s)
		m_e_u->update(gamma);
		m_e_u->addFeatureList(m_grad_u);
		//2. w= w - alpha_v * Grad_u pi(a|s)/pi(a|s) * Grad_u pi(a|s)/pi(a|s)^T * w + alpha_v*td*e_u
		double innerprod = m_grad_u->innerProduct(m_w[i]); //Grad_u pi(a|s)/pi(a|s)^T * w
		m_grad_u->mult(alpha_v*innerprod*-1.0);
		m_grad_u->applyThreshold(0.0001);
		m_w[i]->addFeatureList(m_grad_u);
		m_w[i]->addFeatureList(m_e_u, alpha_v*m_td);
//		m_w[i]->applyThreshold(0.0001);
		//3. u= u + alpha_u * w
		m_pPolicies[i]->addFeatures(m_w[i], alpha_u);
	}
}

void CIncrementalNaturalActorCritic::selectAction(const CState *s, CAction *a)
{
	for (int i = 0; i < m_numPolicies; i++)
	{
		m_pPolicies[i]->selectAction(s, a);
	}
}