#include "actor.h"
#include "vfa.h"
#include "policy.h"
#include "policy-learner.h"
#include "named-var-set.h"
#include "features.h"
#include "etraces.h"
#include "config.h"
#include "parameters-numeric.h"
#include "app.h"

CACLALearner::CACLALearner(ConfigNode* pConfigNode): PolicyLearner(pConfigNode)
{
	m_pStateFeatures = new FeatureList("Actor/s");
	m_pAlpha = CHILD_OBJECT_FACTORY<NumericValue>(pConfigNode, "Alpha", "Learning gain [0..1]");
}

CACLALearner::~CACLALearner()
{
	delete m_pStateFeatures;
}

void CACLALearner::update(const State *s, const Action *a, const State *s_p, double r, double td)
{
	double lastNoise;
	double alpha;

	//CACLA (van Hasselt)
	//if delta>0: theta= theta + alpha*(lastNoise)*phi_pi(s)
	if (td > 0.0)
	{
		alpha = m_pAlpha->get();

		if (alpha != 0.0)
		{
			m_pPolicy->getFeatures(s, m_pStateFeatures);

			lastNoise = a->get(m_pPolicy->getOutputActionIndex()) - m_pPolicy->getDeterministicOutput(m_pStateFeatures);

			m_pPolicy->addFeatures(m_pStateFeatures, alpha*lastNoise);
		}
	}
}

