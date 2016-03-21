#include "stdafx.h"
#include "etraces.h"
#include "experiment.h"
#include "globals.h"
#include "parameters.h"

CLASS_CONSTRUCTOR(CETraces, const char* name) : CFeatureList(name), CParamObject(pParameters)
{
	if (pParameters)
	{
		m_bUse = true;
		CONST_DOUBLE_VALUE(m_threshold,"Threshold",0.001,"Threshold applied to trace factors");
		CONST_DOUBLE_VALUE(m_lambda,"Lambda",0.9,"Lambda parameter");
		const char* replace;
		ENUM_VALUE(replace, Boolean, "Replace", "True","Replace existing traces? Or add?");
		m_bReplaceIfExists = !strcmp(replace, "True");
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
	END_CLASS();
}

CETraces::~CETraces()
{}

void CETraces::update(double factor)
{
	if (!RLSimion::Experiment.isFirstStep() && m_bUse)
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