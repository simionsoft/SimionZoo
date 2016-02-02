#include "stdafx.h"
#include "vfa-actor.h"
#include "vfa.h"
#include "states-and-actions.h"
#include "features.h"

#include "parameter.h"
#include "parameters.h"

CRegularActor::CRegularActor(CParameters *pParameters)
: CSingleOutputPolicyLearner(pParameters)
{
	m_e = new CETraces(pParameters->getChild("ETRACES"));
}

CRegularActor::~CRegularActor()
{
	delete m_e;
}

void CRegularActor::updatePolicy(CState *s, CAction *a, CState *s_p, double r, double td)
{
	double lastNoise;
	double alpha;
	const char* actionVar;

	//Regular gradient actor update
	//theta= theta + alpha*(lastNoise)*phi_pi(s)*td

	alpha = m_pParameters->getParameter("ALPHA")->getDouble();
	actionVar = m_pParameters->getParameter("ACTION")->getStringPtr();

	lastNoise = a->getValue(actionVar) - m_pVFA->getValue(s, a);// m_pOutput->getValue(i);

	m_pVFA->getFeatures(s, a, m_pStateFeatures);

	if (alpha != 0.0)
		m_pVFA->add(m_pStateFeatures, alpha*lastNoise*td);
}

