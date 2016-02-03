#include "stdafx.h"
#include "etraces.h"
#include "parameters.h"
#include "parameter.h"
#include "experiment.h"
#include "globals.h"

CETraces::CETraces(CParameters* pParameters) : CParamObject(pParameters)
{}

CETraces::~CETraces()
{}

void CETraces::update(double factor)
{
	if (!g_pExperiment->m_expProgress.isFirstStep()
		&& m_pParameters->getParameter("USE")->getDouble() == 1.0)
	{
		mult(factor* m_pParameters->getParameter("LAMBDA")->getDouble());
		applyThreshold(m_pParameters->getParameter("THRESHOLD")->getDouble());
	}
	else
		clear();
}

void CETraces::addFeatureList(CFeatureList* inList, double factor)
{
	if (m_pParameters->getParameter("USE")->getDouble() == 1.0)
	{
		bool bReplace = (bool)(1 == (int)m_pParameters->getParameter("REPLACE")->getDouble());
		CFeatureList::addFeatureList(inList, factor, !bReplace, bReplace);
	}
	else
	{
		clear();
		copy(inList);
	}
}