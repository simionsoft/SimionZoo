#include "stdafx.h"
#include "parameters-xml-helper.h"
#include "experiment.h"
#include "globals.h"

static int XMLHelperCountChildren(tinyxml2::XMLElement* pElement, const char* name)
{
	int count = 0;
	tinyxml2::XMLElement* p = pElement->FirstChildElement(name);

	while (p != 0)
	{
		count++;
		p = p->NextSiblingElement(name);
	}
	return count;
}

double XMLHelperGetNumeric(tinyxml2::XMLElement* pElement)
{
	tinyxml2::XMLElement *pChild;

	if (!pElement) return 0.0;

	pChild = pElement->FirstChildElement();

	//simple text value, not numeric
	if (!pChild)
		return atof(pElement->Value());

	//numeric: double
	pChild = pElement->FirstChildElement("decimal");
	if (!pChild)
		return atof(pChild->Value());

	//numeric: schedule
	pChild = pElement->FirstChildElement("schedule");
	if (!pChild)
	{
		const char* interpolation = pChild->FirstChildElement("INTERPOLATION")->Value();
		double initialValue = atof(pChild->FirstChildElement("INITIAL_VALUE")->Value());
		double finalValue = atof(pChild->FirstChildElement("FINAL_VALUE")->Value());
		

		bool eval = g_pExperiment->isEvaluationEpisode();
		double progress;

		if (!eval)
		{
			progress = g_pExperiment->m_expProgress.getExperimentProgress();

			if (!strcmp(interpolation, "linear"))
				return initialValue + progress*(finalValue - initialValue));
			else //quadratic
				return initialValue + (1. - pow(1 - progress, 2.0))*(finalValue - initialValue));
		}

		//evaluation episode
		double evalValue = atof(pChild->FirstChildElement("EVALUATION_EPISODE_VALUE")->Value());
		return evalValue;

	}
	//<schedule>
	//	<INTERPOLATION>linear< / INTERPOLATION>
	//	<INITIAL_VALUE>0.001< / INITIAL_VALUE>
	//	<FINAL_VALUE>0.0001< / FINAL_VALUE>
	//	<EVALUATION_EPISODE_VALUE>0.0< / EVALUATION_EPISODE_VALUE>
	//	< / schedule>

}