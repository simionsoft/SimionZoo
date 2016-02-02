#include "stdafx.h"
#include "critic.h"
#include "vfa.h"
#include "parameters.h"
#include "parameter.h"
#include "features.h"
#include "globals.h"
#include "experiment.h"
#include "vfa-critic.h"

CTDCLambdaCritic::CTDCLambdaCritic(CParameters *pParameters)
	: CVFACritic(pParameters)
{
	m_z= new CETraces(pParameters->getChild("ETRACES"));

	m_s_features = new CFeatureList();
	m_s_p_features = new CFeatureList();
	m_a = new CFeatureList();
	m_b= new CFeatureList();
	m_omega = new CFeatureList();

	m_pAlpha= pParameters->addParameter(CParameter("LEARNING_RATE",0.0));
}

CTDCLambdaCritic::~CTDCLambdaCritic()
{
	delete m_z;
	delete m_b;
	delete m_s_features;
	delete m_s_p_features;
	delete m_a;
	delete m_omega;
}

double CTDCLambdaCritic::updateValue(CState *s, CAction *a, CState *s_p, double r, double rho)
{
	if (m_pAlpha->getDouble()==0.0) return 0.0;
	
	if (g_pExperiment->m_expProgress.isFirstStep())
	{
		m_z->clear();
		m_omega->clear();
	}

	m_pVFA->getFeatures(s, 0, m_s_features);
	m_pVFA->getFeatures(s_p, 0, m_s_p_features);

	//delta= r + gamma*omega(x_{t+1})- omega(x_t)
	double oldValue = m_pVFA->getValue(m_s_features);
	double newValue = m_pVFA->getValue(m_s_p_features);

	double gamma = m_pParameters->getParameter("GAMMA")->getDouble();
	double td= rho*r + gamma * newValue - oldValue;

	//z_{k+1}= rho*gamma*lambda*z_k + omega(x_t)
	m_z->update(rho*gamma);
	m_z->addFeatureList(m_s_features,rho);
	m_z->applyThreshold(0.0001);	


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
	m_pVFA->add(m_z, m_pAlpha->getDouble() *td);
	//theta_{t+1}= theta_t - gamma*rho(1-\lambda)*phi_t*innerprod2

	double lambda = m_z->getParameters()->getParameter("LAMBDA")->getDouble();
	m_pVFA->add(m_s_p_features, -1.0*gamma*rho*(1.0 - lambda)*innerprod2);

	//omega_{t+1}=omega_t+beta(z_{t+1}*td - phi_{t+1}(phi{t+1}^T * omega_t)
	double beta = m_pParameters->getParameter("BETA")->getDouble();
	m_omega->addFeatureList(m_z, beta*td, true, false);
	m_omega->addFeatureList(m_s_p_features,- innerprod1, true, false);
	m_omega->applyThreshold(0.0001);


	return td;
}

