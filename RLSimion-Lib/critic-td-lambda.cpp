#include "stdafx.h"
#include "critic.h"
#include "vfa.h"
#include "features.h"
#include "experiment.h"
#include "vfa-critic.h"
#include "etraces.h"
#include "config.h"
#include "parameters-numeric.h"
#include "app-rlsimion.h"

CTDLambdaCritic::CTDLambdaCritic(CConfigNode* pConfigNode)
	: CCritic(pConfigNode)
{
	m_z = CHILD_OBJECT<CETraces>(pConfigNode, "E-Traces", "Eligibility traces of the critic", true);
	m_z->setName("Critic/E-Traces" );
	m_aux= new CFeatureList("Critic/aux");
	m_pAlpha= CHILD_OBJECT_FACTORY<CNumericValue>(pConfigNode,"Alpha","Learning gain");
}

CTDLambdaCritic::~CTDLambdaCritic()
{
	delete m_aux;
}

double CTDLambdaCritic::update(const CState *s, const CAction *a, const CState *s_p, double r, double rho)
{
	double alpha = m_pAlpha->get();
	if (alpha==0.0) return 0.0;

	//using sample importance
	//z= gamma * lambda * rho * z + phi_v(s)

	double gamma = CSimionApp::get()->pSimGod->getGamma();
	m_z->update(gamma*rho);

	m_pVFunction->getFeatures(s, m_aux);
	m_z->addFeatureList(m_aux,alpha);

	//theta= theta + alpha(r + gamma*v_s_p - v_s)*z
	double v_s= m_pVFunction->get(m_aux,false); //if the v-function has deferred updates, we want to getSample the actual values here

	m_pVFunction->getFeatures(s_p, m_aux);
	double v_s_p= m_pVFunction->get(m_aux);
	double td = rho*r + gamma*v_s_p - v_s;
	
	m_pVFunction->add(m_z.ptr(),td);

	return td;
}

