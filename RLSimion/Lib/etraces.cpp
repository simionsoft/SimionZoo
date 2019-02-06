#include "etraces.h"
#include "experiment.h"
#include "config.h"
#include "app.h"

ETraces::ETraces(ConfigNode* pConfigNode): FeatureList("ETraces")
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
		if (m_bReplace.get()) m_overwriteMode = OverwriteMode::Replace;
		else m_overwriteMode = OverwriteMode::Add;
	}

}

ETraces::ETraces():FeatureList("ETraces")
{
	m_bUse = false;
	m_overwriteMode = OverwriteMode::Replace;
}

ETraces::~ETraces()
{}


void ETraces::update(double factor)
{
	if (!SimionApp::get()->pExperiment->isFirstStep() && m_bUse)
	{
		mult(factor* m_lambda.get());
		applyThreshold(m_threshold.get());
	}
	else
		clear();
}

void ETraces::addFeatureList(FeatureList* inList, double factor)
{
	if (m_bUse)
	{
		FeatureList::addFeatureList(inList, factor);
	}
	else
	{
		clear();
		copyMult(factor,inList);
	}
}