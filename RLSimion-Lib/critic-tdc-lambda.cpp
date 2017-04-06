#include "stdafx.h"
#include "critic.h"
#include "vfa.h"
#include "features.h"
#include "etraces.h"
#include "experiment.h"
#include "vfa-critic.h"
#include "config.h"
#include "parameters-numeric.h"
#include "app.h"

CTDCLambdaCritic::CTDCLambdaCritic(CConfigNode* pConfigNode): CCritic(pConfigNode)
{
	m_z= CHILD_OBJECT<CETraces>(pConfigNode,"E-Traces","Elilgibility traces of the critic",true);
	m_z->setName("Critic/E-Traces");

	m_s_features = new CFeatureList("Critic/s");
	m_s_p_features = new CFeatureList("Critic/s_p");
	m_a = new CFeatureList("Critic/a");
	m_b= new CFeatureList("Critic/b");
	m_omega = new CFeatureList("Critic/omega");

	m_pAlpha= CHILD_OBJECT_FACTORY<CNumericValue>(pConfigNode,"Alpha","Learning gain of the critic");
	m_pBeta = CHILD_OBJECT_FACTORY<CNumericValue>(pConfigNode, "Beta","Learning gain applied to the omega vector");
}

CTDCLambdaCritic::~CTDCLambdaCritic()
{
	delete m_b;
	delete m_s_features;
	delete m_s_p_features;
	delete m_a;
	delete m_omega;
}

double CTDCLambdaCritic::update(const CState *s, const CAction *a, const CState *s_p, double r, double rho)
{
	if (m_pAlpha->get()==0.0) return 0.0;
	
	if (CSimionApp::get()->pExperiment->isFirstStep())
	{
		m_omega->clear();
	}
	//double rho = 1.0;

	m_pVFunction->getFeatures(s, m_s_features);
	m_pVFunction->getFeatures(s_p, m_s_p_features);

	//delta= r + gamma*omega(x_{t+1})- omega(x_t)
	double oldValue = m_pVFunction->get(m_s_features);
	double newValue = m_pVFunction->get(m_s_p_features);

	double gamma = CSimionApp::get()->pSimGod->getGamma();
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
	m_a->copy(m_z.ptr());
	double innerprod2 = m_a->innerProduct(m_omega);
	//theta_{t+1}=theta_t+alpha(z_t*delta_t)
	m_pVFunction->add(m_z.ptr(), m_pAlpha->get() *td);
	//theta_{t+1}= theta_t - gamma*rho(1-\lambda)*phi_t*innerprod2

	double lambda = m_z->getLambda();
	m_pVFunction->add(m_s_p_features, -1.0*gamma*rho*(1.0 - lambda)*innerprod2);

	//omega_{t+1}=omega_t+beta(z_{t+1}*td - phi_{t+1}(phi{t+1}^T * omega_t)
	double beta = m_pBeta->get();
	m_omega->addFeatureList(m_z.ptr(), beta*td);
	m_omega->addFeatureList(m_s_p_features,- innerprod1);
	m_omega->applyThreshold(0.0001);


	return td;
}

