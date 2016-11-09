#include "stdafx.h"
#include "critic.h"
#include "vfa.h"
#include "features.h"
#include "globals.h"
#include "experiment.h"
#include "vfa-critic.h"
#include "etraces.h"
#include "config.h"
#include "parameters-numeric.h"

CLASS_CONSTRUCTOR(CTDLambdaCritic)
	: EXTENDS(CCritic,pParameters)
{
	CHILD_CLASS(m_z, "E-Traces","Eligibility traces of the critic",true, CETraces,"Critic/E-Traces" );
	m_aux= new CFeatureList("Critic/aux");
	NUMERIC_VALUE(m_pAlpha, "Alpha","Learning gain");
	NUMERIC_VALUE(m_pGamma,"Gamma","Gamma parameter");
	END_CLASS();
}

CTDLambdaCritic::~CTDLambdaCritic()
{
	delete m_z;
	delete m_aux;
	delete m_pAlpha;
	delete m_pGamma;
}

double CTDLambdaCritic::updateValue(const CState *s, const CAction *a, const CState *s_p, double r)
{
	double alpha = m_pAlpha->getValue();
	if (alpha==0.0) return 0.0;

	//using sample importance
	//z= gamma * lambda * rho * z + phi_v(s)

	double rho = 1.0;

	double gamma = m_pGamma->getValue();
	m_z->update(gamma*rho);

	m_pVFunction->getFeatures(s, m_aux);
	m_z->addFeatureList(m_aux,alpha);


	//theta= theta + alpha(r + gamma*newValue - oldValue)*z
	double oldValue= m_pVFunction->getValue(m_aux);

	m_pVFunction->getFeatures(s_p, m_aux);
	double newValue= m_pVFunction->getValue(m_aux);
	double td = rho*r + gamma*newValue - oldValue;
	
	m_pVFunction->add(m_z,td);

	return td;
}

