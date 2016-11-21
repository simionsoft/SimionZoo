#include "stdafx.h"
#include "actor.h"
#include "vfa.h"
#include "policy.h"
#include "policy-learner.h"
#include "named-var-set.h"
#include "features.h"
#include "etraces.h"
#include "config.h"
#include "parameters-numeric.h"

CCACLALearner::CCACLALearner(CConfigNode* pConfigNode): CPolicyLearner(pConfigNode)
{
	m_pStateFeatures = new CFeatureList("Actor/s");
	m_e = CHILD_OBJECT<CETraces>(pConfigNode, "E-Traces", "Eligibility traces used by CACLA", true);
	//CHILD_CLASS(m_e, "E-Traces","Eligibility traces used by CACLA",true,CETraces, "Actor/E-Traces");
	m_pAlpha = CHILD_OBJECT_FACTORY<CNumericValue>(pConfigNode, "Alpha", "Learning gain [0..1]");
	//NUMERIC_VALUE(m_pAlpha, "Alpha","Learning gain [0..1]");
}

CCACLALearner::~CCACLALearner()
{
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

		m_pPolicy->getFeatures(s, m_pStateFeatures);

		lastNoise = a->getValue(m_pPolicy->getOutputActionIndex()) - m_pPolicy->getDeterministicOutput(m_pStateFeatures);

		if (alpha != 0.0)
			m_pPolicy->addFeatures(m_pStateFeatures, alpha*lastNoise);
	}
}

