#include "policy.h"
#include "vfa.h"
#include "named-var-set.h"
#include "features.h"
#include "etraces.h"
#include "config.h"
#include "policy-learner.h"
#include "parameters-numeric.h"

RegularPolicyGradientLearner::RegularPolicyGradientLearner(ConfigNode* pConfigNode)
	: PolicyLearner(pConfigNode)
{
	m_pStateFeatures = new FeatureList("Actor/s");
	m_pAlpha = CHILD_OBJECT_FACTORY<NumericValue>(pConfigNode, "Alpha", "The learning gain");
}

RegularPolicyGradientLearner::~RegularPolicyGradientLearner()
{
	delete m_pStateFeatures;
}

void RegularPolicyGradientLearner::update(const State *s, const Action *a, const State *s_p, double r, double td)
{
	double lastNoise;
	double alpha;

	//Regular gradient actor update
	//theta= theta + alpha*(lastNoise)*phi_pi(s)*td

	alpha = m_pAlpha->get();


	m_pPolicy->getFeatures(s, m_pStateFeatures);

	lastNoise = a->get(m_pPolicy->getOutputAction()) - m_pPolicy->getDeterministicOutput(m_pStateFeatures);// m_pOutput->getSample(i);


	if (alpha != 0.0)
		m_pPolicy->addFeatures(m_pStateFeatures, alpha*lastNoise*td);
}

