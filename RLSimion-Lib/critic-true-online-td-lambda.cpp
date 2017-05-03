#include "stdafx.h"
#include "critic.h"
#include "vfa.h"
#include "features.h"
#include "etraces.h"
#include "experiment.h"
#include "vfa-critic.h"
#include "parameters-numeric.h"
#include "config.h"
#include "app.h"

CTrueOnlineTDLambdaCritic::CTrueOnlineTDLambdaCritic(CConfigNode* pConfigNode): CCritic(pConfigNode)
{
	m_e = CHILD_OBJECT<CETraces>(pConfigNode, "E-Traces", "Eligibility traces of the critic", true);
	m_e->setName("Critic/E-Traces" );
	m_aux= new CFeatureList("Critic/aux");
	m_v_s= 0.0;
	m_pAlpha= CHILD_OBJECT_FACTORY<CNumericValue>(pConfigNode, "Alpha", "Learning gain of the critic");
}

CTrueOnlineTDLambdaCritic::~CTrueOnlineTDLambdaCritic()
{
	delete m_aux;
}

double CTrueOnlineTDLambdaCritic::update(const CState *s,const  CAction *a,const CState *s_p, double r, double rho)
{
	double v_s_p;

	if (m_pAlpha->get()==0.0) return 0.0;
	
	if (CSimionApp::get()->pExperiment->isFirstStep())
	{
		//vs= theta^T * phi(s)
		m_pVFunction->getFeatures(s,m_aux);
		m_v_s= m_pVFunction->get(m_aux);
	}
		
	//vs_p= theta^T * phi(s_p)
	m_pVFunction->getFeatures(s_p,m_aux);
	v_s_p= m_pVFunction->get(m_aux);

	double gamma = CSimionApp::get()->pSimGod->getGamma();
	double alpha = m_pAlpha->get();
	//delta= R + gamma* v_s_p - v_s
	double td = r + gamma*v_s_p - m_v_s;

	//e= gamma*lambda*e + alpha*[1-gamma*lambda* e^T*phi(s)]* phi(s)
	m_pVFunction->getFeatures(s,m_aux);										//m_aux <- phi(s)
	double e_T_phi_s= m_e->innerProduct(m_aux);


	m_e->update(gamma);
	double lambda = m_e->getLambda();
	m_e->addFeatureList(m_aux,alpha *(1-gamma*lambda*e_T_phi_s));

	//theta= theta + delta*e + alpha[v_s - theta^T*phi(s)]* phi(s)
	m_pVFunction->add(m_e.ptr(),td);
	double theta_T_phi_s= m_pVFunction->get(m_aux);
	m_pVFunction->add(m_aux,alpha *(m_v_s - theta_T_phi_s));
	//v_s= v_s_p
	m_v_s= v_s_p;

	return td;
}

