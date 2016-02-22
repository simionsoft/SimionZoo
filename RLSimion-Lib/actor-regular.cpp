#include "stdafx.h"
#include "vfa-actor.h"
#include "vfa.h"
#include "states-and-actions.h"
#include "features.h"
#include "etraces.h"
#include "xml-parameters.h"
#include "vfa-policy.h"

CRegularPolicyGradientLearner::CRegularPolicyGradientLearner(tinyxml2::XMLElement *pParameters)
: CSingleOutputVFAPolicyLearner(pParameters->FirstChildElement("Policy")->FirstChildElement())
{
	m_pStateFeatures = new CFeatureList();
	m_e = new CETraces(pParameters->FirstChildElement("E-Traces"));
	m_pAlpha = XMLParameters::getNumericHandler(pParameters->FirstChildElement("Alpha"));
}

CRegularPolicyGradientLearner::~CRegularPolicyGradientLearner()
{
	delete m_pStateFeatures;
	delete m_e;
	delete m_pAlpha;
}

void CRegularPolicyGradientLearner::updatePolicy(CState *s, CAction *a, CState *s_p, double r, double td)
{
	double lastNoise;
	double alpha;

	//Regular gradient actor update
	//theta= theta + alpha*(lastNoise)*phi_pi(s)*td

	alpha = m_pAlpha->getValue();


	m_pPolicy->getVFA()->getFeatures(s, a, m_pStateFeatures);

	lastNoise = a->getValue(m_pPolicy->getOutputActionIndex()) - m_pPolicy->getVFA()->getValue(m_pStateFeatures);// m_pOutput->getValue(i);


	if (alpha != 0.0)
		m_pPolicy->getVFA()->add(m_pStateFeatures, alpha*lastNoise*td);
}

