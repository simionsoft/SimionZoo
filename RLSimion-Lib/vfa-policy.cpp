#include "stdafx.h"
#include "states-and-actions.h"
#include "noise.h"
#include "vfa.h"
#include "vfa-policy.h"
#include "xml-parameters.h"
#include "globals.h"
#include "world.h"


CSingleOutputVFAPolicy::CSingleOutputVFAPolicy(tinyxml2::XMLElement* pParameters)
: CParamObject(pParameters)
{
	m_pVFA = new CLinearVFA(pParameters->FirstChildElement("Linear-VFA"));

	m_pExpNoise = CNoise::getInstance (pParameters->FirstChildElement("Exploration-Noise"));

	m_outputAction = XMLParameters::getConstString(m_pParameters->FirstChildElement("Output-Action"));

	CAction *pActionDescriptor = RLSimion::g_pWorld->getActionDescriptor();
	m_outputActionIndex = pActionDescriptor->getVarIndex(m_outputAction);

	m_pVFA->saturateOutput(pActionDescriptor->getMin(m_outputActionIndex), pActionDescriptor->getMax(m_outputActionIndex));
}

CSingleOutputVFAPolicy::~CSingleOutputVFAPolicy()
{
	delete m_pVFA;
	delete m_pExpNoise;
}

void CSingleOutputVFAPolicy::selectAction(CState *s, CAction *a)
{
	double noise;
	double output;

	
	noise = m_pExpNoise->getValue();

	output = m_pVFA->getValue(s, 0);

	a->setValue(m_outputActionIndex, output + noise);
}