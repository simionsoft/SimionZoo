#include "stdafx.h"
#include "etraces.h"
#include "experiment.h"
#include "globals.h"

CETraces::CETraces(tinyxml2::XMLElement* pParameters) : CParamObject(pParameters)
{
	m_bUse = atoi(m_pParameters->FirstChildElement("Use")->Value()) != 0;
	m_threshold = atof(m_pParameters->FirstChildElement("Threshold")->Value());
	m_lambda = atof(m_pParameters->FirstChildElement("Lambda")->Value());
	m_bReplace = atoi(m_pParameters->FirstChildElement("Replace")->Value()) != 0;
}

CETraces::~CETraces()
{}

void CETraces::update(double factor)
{
	if (!g_pExperiment->m_expProgress.isFirstStep() && m_bUse)
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
		CFeatureList::addFeatureList(inList, factor, !m_bReplace, m_bReplace);
	}
	else
	{
		clear();
		copy(inList);
	}
}