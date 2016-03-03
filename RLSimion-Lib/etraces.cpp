#include "stdafx.h"
#include "etraces.h"
#include "experiment.h"
#include "globals.h"
#include "parameters.h"

CETraces::CETraces(const char* name, CParameters* pParameters) : CFeatureList(name), CParamObject(pParameters)
{
	if (pParameters)
	{
		m_bUse = true;
		m_threshold = m_pParameters->getConstDouble("Threshold");
		m_lambda = m_pParameters->getConstDouble("Lambda");
		m_bReplaceIfExists = m_pParameters->getConstBoolean("Replace");
		m_bAddIfExists = !m_bReplaceIfExists;
	}
	else
	{
		m_bUse = false;
		m_threshold = 0.0;
		m_lambda = 1.0;
		m_bReplaceIfExists = false;
		m_bAddIfExists = false;
	}
}

CETraces::~CETraces()
{}

void CETraces::update(double factor)
{
	if (!RLSimion::g_pExperiment->m_expProgress.isFirstStep() && m_bUse)
	{
		mult(factor* m_lambda);
		applyThreshold(m_threshold);
	}
	else
		clear();
}

void CETraces::addFeatureList(CFeatureList* inList, double factor)
{
	if (m_bUse)
	{
		CFeatureList::addFeatureList(inList, factor);
	}
	else
	{
		clear();
		copy(inList);
	}
}