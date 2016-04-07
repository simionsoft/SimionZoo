#include "stdafx.h"
#include "critic.h"
#include "vfa.h"
#include "features.h"
#include "etraces.h"
#include "globals.h"
#include "experiment.h"
#include "vfa-critic.h"
#include "parameters-numeric.h"
#include "parameters.h"

CLASS_CONSTRUCTOR(CTrueOnlineTDLambdaCritic)
	: EXTENDS(CCritic,pParameters)
{
	CHILD_CLASS(m_e, "E-Traces","Eligibility traces of the critic",true, CETraces,"Critic/E-Traces" );
	m_aux= new CFeatureList("Critic/aux");
	m_v_s= 0.0;
	NUMERIC_VALUE(m_pAlpha, "Alpha", "Learning gain of the critic");
	NUMERIC_VALUE(m_pGamma, "Gamma","Gamma parameter");
	END_CLASS();
}

CTrueOnlineTDLambdaCritic::~CTrueOnlineTDLambdaCritic()
{
	delete m_e;
	delete m_aux;
	delete m_pAlpha;
	delete m_pGamma;
}

double CTrueOnlineTDLambdaCritic::updateValue(const CState *s,const  CAction *a,const CState *s_p, double r)
{
	double v_s_p;

	if (m_pAlpha->getValue()==0.0) return 0.0;
	double rho = 0.0;
	
	if (RLSimion::Experiment.isFirstStep())
	{
		//vs= theta^T * phi(s)
		m_pVFunction->getFeatures(s,m_aux);
		m_v_s= m_pVFunction->getValue(m_aux);
	}
		
	//vs_p= theta^T * phi(s_p)
	m_pVFunction->getFeatures(s_p,m_aux);
	v_s_p= m_pVFunction->getValue(m_aux);

	double gamma = m_pGamma->getValue();
	double alpha = m_pAlpha->getValue();
	//delta= R + gamma* v_s_p - v_s
	double td = r + gamma*v_s_p - m_v_s;

	//e= gamma*lambda*e + alpha*[1-gamma*lambda* e^T*phi(s)]* phi(s)
	m_pVFunction->getFeatures(s,m_aux);										//m_aux <- phi(s)
	double e_T_phi_s= m_e->innerProduct(m_aux);


	m_e->update(gamma);
	double lambda = m_e->getLambda();
	m_e->addFeatureList(m_aux,alpha *(1-gamma*lambda*e_T_phi_s));

	//theta= theta + delta*e + alpha[v_s - theta^T*phi(s)]* phi(s)
	m_pVFunction->add(m_e,td);
	double theta_T_phi_s= m_pVFunction->getValue(m_aux);
	m_pVFunction->add(m_aux,alpha *(m_v_s - theta_T_phi_s));
	//v_s= v_s_p
	m_v_s= v_s_p;

	return td;
}

