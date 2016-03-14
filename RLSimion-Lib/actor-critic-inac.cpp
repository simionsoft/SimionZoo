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
/*
CLASS_CONSTRUCTOR(CIncrementalNaturalActorCritic)(CParameters* pParameters) : CParamObject(pParameters)
{
	m_s_features = new CFeatureList("Critic/s");
	m_s_p_features = new CFeatureList("Critic/s_p");
	NUMERIC_VALUE(m_pAlphaV, pParameters,"Alpha-v");
	NUMERIC_VALUE(m_pAlphaR,pParameters, "Alpha-r");
	NUMERIC_VALUE(m_pGamma, pParameters,"Gamma");
	CHILD_CLASS(m_e_v,"V-ETraces",CETraces,"Critic/e_v",pParameters->getChild("E-Traces"));


	m_grad_u = new CFeatureList("Actor/grad-u");

	NUMERIC_VALUE(m_pGamma, pParameters, "Gamma");
	NUMERIC_VALUE(m_pAlphaU, pParameters, "Alpha-u");

	CHILD_CLASS(m_e, "U-ETraces", CETraces, "Actor/E-Traces", pParameters->getChild("E-Traces"));
	m_w = new CFeatureList("Actor/w");
	END_CLASS();
}

CIncrementalNaturalActorCritic::~CIncrementalNaturalActorCritic()
{
	delete m_s_features;
	delete m_s_p_features;

	delete m_pAlphaV;
	delete m_pAlphaR;
	delete m_pGamma;

	delete m_e_v;

	delete m_grad_u;
	delete m_s_features;

	delete m_pGamma;
	delete m_pAlphaU;

	delete m_e;
	delete m_w;
}

double CIncrementalNaturalActorCritic::updateValue(const CState *s, const CAction *a, const CState *s_p, double r)
{
	// Incremental Natural Actor - Critic(INAC)
	//Critic update:
	//td= r - avg_r + gamma*V(s_p) - V(s)
	//avg_r= avg_r + alpha_r * td
	//e_v= gamma* lambda*e_v + phi(s)
	//v = v + alpha_v*td*e_v
	double alpha_v = m_pAlphaV->getValue();
	double gamma = m_pGamma->getValue();
	m_pVFA->getFeatures(s, m_s_features);
	m_pVFA->getFeatures(s_p, m_s_p_features);
	//1. td= r - avg_r + gamma*V(s_p) - V(s)
	double td = r - m_avg_r + gamma * m_pVFA->getValue(m_s_p_features)
		- m_pVFA->getValue(m_s_features);

	//2. avg_r= avg_r + alpha_r * td
	m_avg_r += td * m_pAlphaR->getValue();

	//3. e_v= gamma* lambda*e_v + phi(s)
	m_e_v->update(gamma);
	m_e_v->addFeatureList(m_s_features);
	//4. v = v + alpha_v*td*e_v
	m_pVFA->add(m_e_v, alpha_v*td);

	return td;
}



void CIncrementalNaturalActorCritic::updatePolicy(const CState* s, const CState* a, const CState *s_p, double r, double td)
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

	if (RLSimion::g_pExperiment->isFirstStep())
		m_w->clear();
	
	m_pPolicy->getGradient(s,a,m_grad_u);
	
	//1. e_u= gamma*lambda*e_u + Grad_u pi(a|s)/pi(a|s)
	m_e->update(gamma);
	m_e->addFeatureList(m_grad_u);
	//2. w= w - alpha_v * Grad_u pi(a|s)/pi(a|s) * Grad_u pi(a|s)/pi(a|s)^T * w + alpha_v*td*e_u
	double innerprod = m_grad_u->innerProduct(m_w); //Grad_u pi(a|s)/pi(a|s)^T * w
	m_grad_u->mult(alpha_v*innerprod*-1.0);
	m_grad_u->applyThreshold(0.0001);
	m_w->addFeatureList(m_grad_u);
	m_w->addFeatureList(m_e, alpha_v*td);
	m_w->applyThreshold(0.0001);
	//3. u= u + alpha_u * w
	m_pPolicy->getVFA()->add(m_w, alpha_u);
}
*/