#include "stdafx.h"
#include "states-and-actions.h"
#include "noise.h"
#include "vfa-actor.h"
#include "vfa.h"
#include "vfa-policy.h"
#include "actor.h"
#include "xml-parameters.h"
#include "globals.h"
#include "world.h"


CSingleOutputVFAPolicy::CSingleOutputVFAPolicy(tinyxml2::XMLElement* pParameters)
: CParamObject(pParameters->FirstChildElement("State-Function-Gaussian-Noise"))
{
	m_pVFA = new CLinearVFA(pParameters->FirstChildElement("Linear-VFA"));

	m_pExpNoise = new CGaussianNoise(pParameters->FirstChildElement("Exploration-Noise"));
	m_outputAction = pParameters->FirstChildElement("Output-Action")->GetText();
	CAction *pActionDescriptor = g_pWorld->getActionDescriptor();
	m_outputActionIndex = pActionDescriptor->getVarIndex(m_outputAction);

	m_pVFA->saturateOutput(pActionDescriptor->getMin(m_outputActionIndex)
		, pActionDescriptor->getMax(m_outputActionIndex));
}

CSingleOutputVFAPolicy::~CSingleOutputVFAPolicy()
{
	delete m_pVFA;
	delete m_pExpNoise;
}

void CSingleOutputVFAPolicy::selectAction(CState *s, CAction *a)
{
	double a_width;
	double noise;
	double output;

	//scaling the noise after sampling the normal distribution seems to mess with INAC's use of the variance
	//a_width = 0.5*(a->getMax(m_outputActionIndex) - a->getMin(m_outputActionIndex));
	noise = m_pExpNoise->getNewValue();// *a_width;

	output = m_pVFA->getValue(s, 0);

	double finalOutput = noise + output;
	finalOutput = std::max(a->getMin(m_outputActionIndex), std::min(a->getMax(m_outputActionIndex), finalOutput));

	a->setValue(m_outputActionIndex, output + noise);
}