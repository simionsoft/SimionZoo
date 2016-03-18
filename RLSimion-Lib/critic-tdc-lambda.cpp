#include "stdafx.h"
#include "critic.h"
#include "vfa.h"
#include "features.h"
#include "etraces.h"
#include "globals.h"
#include "experiment.h"
#include "vfa-critic.h"
#include "parameters.h"
#include "parameters-numeric.h"

CLASS_CONSTRUCTOR(CTDCLambdaCritic)
	: CCritic(pParameters)
{
	CHILD_CLASS(m_z,"E-Traces",CETraces,"Critic/E-Traces");

	m_s_features = new CFeatureList("Critic/s");
	m_s_p_features = new CFeatureList("Critic/s_p");
	m_a = new CFeatureList("Critic/a");
	m_b= new CFeatureList("Critic/b");
	m_omega = new CFeatureList("Critic/omega");

	NUMERIC_VALUE(m_pAlpha,pParameters,"Alpha");
	NUMERIC_VALUE(m_pBeta,pParameters,"Beta");
	NUMERIC_VALUE(m_pGamma,pParameters,"Gamma");
	END_CLASS();
}

CTDCLambdaCritic::~CTDCLambdaCritic()
{
	delete m_z;
	delete m_b;
	delete m_s_features;
	delete m_s_p_features;
	delete m_a;
	delete m_omega;
	delete m_pAlpha;
	delete m_pBeta;
	delete m_pGamma;
}

double CTDCLambdaCritic::updateValue(const CState *s, const CAction *a, const CState *s_p, double r)
{
	if (m_pAlpha->getValue()==0.0) return 0.0;
	
	if (RLSimion::g_pExperiment->isFirstStep())
	{
		m_omega->clear();
	}
	double rho = 1.0;

	m_pVFunction->getFeatures(s, m_s_features);
	m_pVFunction->getFeatures(s_p, m_s_p_features);

	//delta= r + gamma*omega(x_{t+1})- omega(x_t)
	double oldValue = m_pVFunction->getValue(m_s_features);
	double newValue = m_pVFunction->getValue(m_s_p_features);

	double gamma = m_pGamma->getValue();
	double td= rho*r + gamma * newValue - oldValue;

	//z_{k+1}= rho*gamma*lambda*z_k + omega(x_t)
	m_z->update(rho*gamma);
	m_z->addFeatureList(m_s_features,rho);

	//\theta_{t+1}=\theta_{t}+\alpha(z_t*delta_t-gamma*rho(1-\lambda)\phi_t*(z_{t+1}^T*w_t))
	//innerprod1= omega(x_t)^T*w //<----- used in the update of w
	m_a->clear();
	m_a->copy(m_s_p_features);
	double innerprod1= m_a->innerProduct(m_omega);
	//innerprod2= z_{t+1}^T*w_t
	m_a->clear();
	m_a->copy(m_z);
	double innerprod2 = m_a->innerProduct(m_omega);
	//theta_{t+1}=theta_t+alpha(z_t*delta_t)
	m_pVFunction->add(m_z, m_pAlpha->getValue() *td);
	//theta_{t+1}= theta_t - gamma*rho(1-\lambda)*phi_t*innerprod2

	double lambda = m_z->getLambda();
	m_pVFunction->add(m_s_p_features, -1.0*gamma*rho*(1.0 - lambda)*innerprod2);

	//omega_{t+1}=omega_t+beta(z_{t+1}*td - phi_{t+1}(phi{t+1}^T * omega_t)
	double beta = m_pBeta->getValue();
	m_omega->addFeatureList(m_z, beta*td);
	m_omega->addFeatureList(m_s_p_features,- innerprod1);
	m_omega->applyThreshold(0.0001);


	return td;
}

