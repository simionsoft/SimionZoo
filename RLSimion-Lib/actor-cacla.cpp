#include "stdafx.h"
#include "actor.h"
#include "vfa.h"
#include "vfa-actor.h"
#include "vfa-policy.h"
#include "named-var-set.h"
#include "features.h"
#include "etraces.h"
#include "parameters.h"

CCACLALearner::CCACLALearner(CParameters *pParameters)
: CSingleOutputVFAPolicyLearner(pParameters->getChild("VFA-Policy"))
{
	m_pStateFeatures = new CFeatureList("Actor\\s");
	m_e = new CETraces("Actor\\E-Traces",pParameters->getChild("E-Traces"));
	m_pAlpha = pParameters->getNumericHandler("Alpha");
}

CCACLALearner::~CCACLALearner()
{
	delete m_pAlpha;
	delete m_e;
	delete m_pStateFeatures;
}

void CCACLALearner::updatePolicy(const CState *s, const CAction *a, const CState *s_p, double r, double td)
{
	double lastNoise;
	double alpha;


	//CACLA (van Hasselt)
	//if delta>0: theta= theta + alpha*(lastNoise)*phi_pi(s)

	if (td > 0.0)
	{
		alpha = m_pAlpha->getValue();

		m_pPolicy->getVFA()->getFeatures(s, m_pStateFeatures);

		lastNoise = a->getValue(m_pPolicy->getOutputActionIndex()) - m_pPolicy->getVFA()->getValue(m_pStateFeatures);

		if (alpha != 0.0)
			m_pPolicy->getVFA()->add(m_pStateFeatures, alpha*lastNoise);
	}
}

