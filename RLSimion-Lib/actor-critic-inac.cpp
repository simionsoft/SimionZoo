#include "stdafx.h"
//#include "vfa-actor-critic.h"
#include "experiment.h"
#include "globals.h"
#include "actor.h"
#include "features.h"
#include "etraces.h"
#include "vfa.h"
#include "vfa-policy.h"
#include "vfa-actor.h"
#include "noise.h"
#include "states-and-actions.h"
#include "vfa-critic.h"
#include "xml-parameters.h"

CIncrementalNaturalCritic::CIncrementalNaturalCritic(tinyxml2::XMLElement* pParameters) : CVFACritic(pParameters)
{
	m_s_features = new CFeatureList();
	m_s_p_features = new CFeatureList();
	m_pAlphaV = XMLParameters::getNumericHandler(pParameters->FirstChildElement("Alpha-v"));
	m_pAlphaR = XMLParameters::getNumericHandler(pParameters->FirstChildElement("Alpha-r"));
	m_pGamma = XMLParameters::getNumericHandler(pParameters->FirstChildElement("Gamma"));
	m_e_v = new CETraces(pParameters->FirstChildElement("E-Traces"));
}

CIncrementalNaturalCritic::~CIncrementalNaturalCritic()
{
	delete m_s_features;
	delete m_s_p_features;

	delete m_pAlphaV;
	delete m_pAlphaR;
	delete m_pGamma;

	delete m_e_v;
}

double CIncrementalNaturalCritic::updateValue(CState *s, CAction *a, CState *s_p, double r, double rho)
{
	// Incremental Natural Actor - Critic(INAC)
	//Critic update:
	//td= r - avg_r + gamma*V(s_p) - V(s)
	//avg_r= avg_r + alpha_r * td
	//e_v= gamma* lambda*e_v + phi(s)
	//v = v + alpha_v*td*e_v
	double alpha_v = m_pAlphaV->getValue();
	double gamma = m_pGamma->getValue();
	m_pVFA->getFeatures(s, a, m_s_features);
	m_pVFA->getFeatures(s_p, a, m_s_p_features);
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

CIncrementalNaturalActor::CIncrementalNaturalActor(tinyxml2::XMLElement* pParameters) 
							: CSingleOutputVFAPolicyLearner(pParameters)
{
	m_grad_u = new CFeatureList();
	m_s_features = new CFeatureList();
	m_pGamma = XMLParameters::getNumericHandler(pParameters->FirstChildElement("Gamma"));

	m_pAlphaU = XMLParameters::getNumericHandler(pParameters->FirstChildElement("Alpha-u"));
	m_pAlphaV = XMLParameters::getNumericHandler(pParameters->FirstChildElement("Alpha-v"));

	m_e = new CETraces(pParameters->FirstChildElement("E-Traces"));
	m_w = new CFeatureList();
}

CIncrementalNaturalActor::~CIncrementalNaturalActor()
{
	delete m_grad_u;
	delete m_s_features;

	delete m_pGamma;
	delete m_pAlphaU;
	delete m_pAlphaV;

	delete m_e;
	delete m_w;
}

void CIncrementalNaturalActor::updatePolicy(CState* s, CState* a, CState *s_p, double r, double td)
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
	

	//0. Grad_u pi(a|s)/pi(a|s) = (a - pi(s)) * phi(s) / sigma*2
	m_pPolicy->getVFA()->getFeatures(s, a, m_s_features);
	double sigma = m_pPolicy->getExpNoise()->getSigma();
	double factor = (a->getValue(m_pPolicy->getOutputActionIndex())
		- m_pPolicy->getVFA()->getValue(m_s_features)) / (sigma*sigma);
	m_grad_u->clear();
	m_grad_u->addFeatureList(m_s_features, factor);
	//1. e_u= gamma*lambda*e_u + Grad_u pi(a|s)/pi(a|s)
	m_e->update(gamma);
	m_e->addFeatureList(m_grad_u);
	//2. w= w - alpha_v * Grad_u pi(a|s)/pi(a|s) * Grad_u pi(a|s)/pi(a|s)^T * w + alpha_v*td*e_u
	double innerprod = m_grad_u->innerProduct(m_w); //Grad_u pi(a|s)/pi(a|s)^T * w
	m_grad_u->mult(alpha_v*innerprod*-1.0);
	m_w->addFeatureList(m_grad_u);
	m_w->addFeatureList(m_e, alpha_v*td);
	//3. u= u + alpha_u * w
	m_pPolicy->getVFA()->add(m_w, alpha_u);
}
/*
CIncrementalNaturalActorCritic::CIncrementalNaturalActorCritic(tinyxml2::XMLElement* pParameters)
: CVFAActorCritic(pParameters)
{
	m_grad_u= new CFeatureList();
	m_s_features= new CFeatureList();
	m_s_p_features = new CFeatureList();
	m_e_u = new CETraces*[m_numOutputs];
	m_w = new CFeatureList*[m_numOutputs];
	for (int i = 0; i < m_numOutputs; i++)
	{
		m_e_u[i] = new CETraces(pParameters->getChildByTag("ETRACES_U", i));
		m_w[i] = new CFeatureList();
	}

	m_e_v= new CETraces(pParameters->getChild("ETRACES_V"));
}

CIncrementalNaturalActorCritic::~CIncrementalNaturalActorCritic()
{
	delete m_grad_u;
	delete m_s_features;
	delete m_s_p_features;
	delete m_e_u;
	delete m_e_v;
}

double CIncrementalNaturalCritic::updateValue(CState *s, CAction *a, CState *s_p, double r, double rho)
{
	// Incremental Natural Actor - Critic(INAC)
	//Critic update:
	//td= r - avg_r + gamma*V(s_p) - V(s)
	//avg_r= avg_r + alpha_r * td
	//e_v= gamma* lambda*e_v + phi(s)
	//v = v + alpha_v*td*e_v
	double alpha_v = m_pParameters->getParameter("ALPHA_V")->getDouble();
	double gamma = m_pParameters->getParameter("GAMMA")->getDouble();
	m_pVFunction->getFeatures(s, a, m_s_features);
	m_pVFunction->getFeatures(s_p, a, m_s_p_features);
	//1. td= r - avg_r + gamma*V(s_p) - V(s)
	double td = r - m_avg_r + gamma * m_pVFunction->getValue(m_s_p_features) 
		- m_pVFunction->getValue(m_s_features);

	//2. avg_r= avg_r + alpha_r * td
	m_avg_r += m_pParameters->getParameter("ALPHA_R")->getDouble();

	//3. e_v= gamma* lambda*e_v + phi(s)
	m_e_v->update(gamma);
	m_e_v->addFeatureList(m_s_features);
	//4. v = v + alpha_v*td*e_v
	m_pVFunction->add(m_e_v, alpha_v*td);

	return td;
}

void CIncrementalNaturalActorCritic::updatePolicy(CState *s, CAction *a, CState *s_p, double r, double td)
{
	const char* actionVar;
	double gamma = m_pParameters->getParameter("GAMMA")->getDouble();
	double alpha_v = m_pParameters->getParameter("ALPHA_V")->getDouble();

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

	for (int i = 0; i < m_numOutputs; i++)
	{
		double alpha_u = m_pParameters->getChildByTag("VFA-Policy-Learner",i)->getParameter("ALPHA_U")->getDouble();

		actionVar = m_pParameters->getParameter("ACTION")->getStringPtr();

		//0. Grad_u pi(a|s)/pi(a|s) = (a - pi(s)) * phi(s) / sigma*2
		m_pPolicies[i]->getVFA()->getFeatures(s, a, m_s_features);
		double sigma = m_pPolicies[i]->getExpNoise()->getSigma();
		double factor = (a->getValue(actionVar) 
			- m_pPolicies[i]->getVFA()->getValue(m_s_features)) / (sigma*sigma);
		m_grad_u->clear();
		m_grad_u->addFeatureList(m_s_features, factor);
		//1. e_u= gamma*lambda*e_u + Grad_u pi(a|s)/pi(a|s)
		m_e_u[i]->update(gamma);
		m_e_u[i]->addFeatureList(m_grad_u);
		//2. w= w - alpha_v * Grad_u pi(a|s)/pi(a|s) * Grad_u pi(a|s)/pi(a|s)^T * w + alpha_v*td*e_u
		double innerprod = m_grad_u->innerProduct(m_w[i]); //Grad_u pi(a|s)/pi(a|s)^T * w
		m_grad_u->mult(alpha_v*innerprod*-1.0);
		m_w[i]->addFeatureList(m_grad_u);
		m_w[i]->addFeatureList(m_e_u[i], alpha_v*td);
		//3. u= u + alpha_u * w
		m_pPolicies[i]->getVFA()->add(m_w[i], alpha_u);
	}
}*/