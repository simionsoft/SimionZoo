#include "stdafx.h"
#include "vfa-actor.h"
#include "vfa.h"

#include "states-and-actions.h"
#include "features.h"

#include "parameter.h"
#include "parameters.h"


//The compatible features of a gaussian policy, according to
//http://www0.cs.ucl.ac.uk/staff/d.silver/web/Teaching_files/pg.pdf
//is:
// \Psi_sa= \delta log pi(s,a)= (a_t-\pi(s_t)) * phi(s_t) / sigma^2


CIncrementalNaturalActor::CIncrementalNaturalActor(CParameters *pParameters) 
	: CSingleOutputPolicyLearner(pParameters)
{
	m_e = new CETraces(pParameters->getChild("ETRACES"));
}

CIncrementalNaturalActor::~CIncrementalNaturalActor()
{
	delete m_e;
}

void CIncrementalNaturalActor::updatePolicy(CState *s, CAction *a, CState *s_p, double r, double td)
{
	double lastNoise;
	double alpha;
	const char* actionVar;

	//Incremental Natural Actor (INAC)
	//e_u= gamma*lambda*e_u + Grad_u pi(a|s)/pi(a|s)
	//w= w - alpha_u * Grad_u pi(a|s)/pi(a|s) * Grad_u pi(a|s)/pi(a|s)^T * w + alpha_v*td*e_u
	//u= u + alpha_u * w

	//If we use Gaussian Noise added to the VFA's output:
	//Grad_u pi(a|s)/pi(a|s) = (a - pi(s)) * phi(s) / sigma*2


	alpha = m_pParameters->getParameter("ALPHA")->getDouble();
	actionVar = m_pParameters->getParameter("ACTION")->getStringPtr();

	lastNoise = a->getValue(actionVar) - m_pVFA->getValue(s, a);// m_pOutput->getValue(i);

	m_pVFA->getFeatures(s, a, m_pStateFeatures);

	if (alpha != 0.0)
		m_pVFA->add(m_pStateFeatures, alpha*lastNoise);
}