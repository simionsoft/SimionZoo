#include "stdafx.h"
#include "actor.h"
#include "vfa.h"
#include "vfa-actor.h"
#include "vfa-policy.h"
#include "states-and-actions.h"
#include "features.h"
#include "etraces.h"
#include "xml-parameters.h"

CCACLALearner::CCACLALearner(tinyxml2::XMLElement *pParameters)
: CSingleOutputVFAPolicyLearner(pParameters->FirstChildElement("Policy")->FirstChildElement())
{
	m_pStateFeatures = new CFeatureList();
	m_e = new CETraces(pParameters->FirstChildElement("E-Traces"));
	m_pAlpha = XMLParameters::getNumericHandler(pParameters->FirstChildElement("Alpha"));
}

CCACLALearner::~CCACLALearner()
{
	delete m_pAlpha;
	delete m_e;
	delete m_pStateFeatures;
}

void CCACLALearner::updatePolicy(CState *s, CAction *a, CState *s_p, double r, double td)
{
	double lastNoise;
	double alpha;


	//CACLA (van Hasselt)
	//if delta>0: theta= theta + alpha*(lastNoise)*phi_pi(s)

	if (td > 0.0)
	{
		alpha = m_pAlpha->getValue();

		m_pPolicy->getVFA()->getFeatures(s, a, m_pStateFeatures);

		lastNoise = a->getValue(m_pPolicy->getOutputActionIndex()) - m_pPolicy->getVFA()->getValue(m_pStateFeatures);

		if (alpha != 0.0)
			m_pPolicy->getVFA()->add(m_pStateFeatures, alpha*lastNoise);
	}
}

