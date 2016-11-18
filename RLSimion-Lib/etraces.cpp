#include "stdafx.h"
#include "etraces.h"
#include "experiment.h"
#include "globals.h"
#include "config.h"
#include "app.h"

CETraces::CETraces(CConfigNode* pConfigNode) : CFeatureList()
{
	if (pConfigNode)
	{
		m_bUse = true;
		m_threshold = DOUBLE_PARAM(pConfigNode,"Threshold", "Threshold applied to trace factors", 0.001);
		//CONST_DOUBLE_VALUE(m_threshold,"Threshold",0.001,"Threshold applied to trace factors");
		m_lambda = DOUBLE_PARAM(pConfigNode,"Lambda", "Lambda parameter", 0.9);
		//CONST_DOUBLE_VALUE(m_lambda,"Lambda",0.9,"Lambda parameter");
		m_bReplace= BOOL_PARAM(pConfigNode,"Replace", "Replace existing traces? Or add?",true);
		//ENUM_VALUE(replace, Boolean, "Replace", "True","Replace existing traces? Or add?");
		m_bReplaceIfExists = m_bReplace.get();
		m_bAddIfExists = !m_bReplaceIfExists;
	}
	else
	{
		m_bUse = false;

		m_bReplaceIfExists = false;
		m_bAddIfExists = false;
	}
	END_CLASS();
}

CETraces::~CETraces()
{}


void CETraces::update(double factor)
{
	if (!CSimionApp::get()->pExperiment->isFirstStep() && m_bUse)
	{
		mult(factor* m_lambda.get());
		applyThreshold(m_threshold.get());
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