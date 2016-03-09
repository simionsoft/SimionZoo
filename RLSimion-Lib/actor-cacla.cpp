#include "stdafx.h"
#include "actor.h"
#include "vfa.h"
#include "vfa-actor.h"
#include "vfa-policy.h"
#include "named-var-set.h"
#include "features.h"
#include "etraces.h"
#include "parameters.h"
#include "globals.h"

CLASS_CONSTRUCTOR(CCACLALearner)(CParameters *pParameters): EXTENDS(CVFAPolicyLearner,pParameters->getChild("VFA-Policy"))
{
	m_pStateFeatures = new CFeatureList("Actor/s");
	CHILD_CLASS(m_e, "E-Traces",CETraces, "Actor/E-Traces", pParameters->getChild("E-Traces"));
	/*m_e = new CETraces("Actor/E-Traces",pParameters->getChild("E-Traces"));*/
	NUMERIC_VALUE(m_pAlpha, pParameters, "Alpha");

	END_CLASS();
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

