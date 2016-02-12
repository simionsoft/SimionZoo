#include "stdafx.h"
#include "parameters-xml-helper.h"
#include "globals.h"
#include "experiment.h"

class CConstantValue : public INumericValue
{
	double m_value;
public:
	CConstantValue(tinyxml2::XMLElement* pParameters)
	{
		//<ALPHA>0.1</ALPHA>
		m_value = atof(pParameters->Value());
	}
	double get(){ return m_value; }
};

enum EnumInterpolation{linear, quadratic};
enum EnumTimeReference{ experiment,episode };

class CInterpolatedValue : public INumericValue
{
	double m_startValue;
	double m_endValue;
	double m_evaluationValue;
	EnumInterpolation m_interpolation;
	EnumTimeReference m_timeReference;
public:
	CInterpolatedValue(tinyxml2::XMLElement* pParameters);
	double get();
};

CInterpolatedValue::CInterpolatedValue(tinyxml2::XMLElement* pParameters)
{
	tinyxml2::XMLElement* param;

	param = pParameters->FirstChildElement("Interpolation");
	assert(param);
	if (!strcmp("linear", param->Value())) m_interpolation = linear;
	else if (!strcmp("quadratic", param->Value())) m_interpolation = quadratic;
	else assert(0);

	param = pParameters->FirstChildElement("Time-reference");

	if (!param) m_timeReference = episode;
	else
	{
		if (!strcmp("experiment", param->Value())) m_timeReference = experiment;
		else if (!strcmp("episode", param->Value())) m_timeReference = episode;
		else assert(0);
	}

	param = pParameters->FirstChildElement("Initial-Value");
	if (param) m_startValue = atof(param->Value());
	else assert(0);

	param = pParameters->FirstChildElement("Final-Value");
	if (param) m_endValue = atof(param->Value());
	else assert(0);

	param = pParameters->FirstChildElement("Evaluation-Value");
	if (param) m_evaluationValue = atof(param->Value());
	else assert(0);
	//<ALPHA>
	//	<INTERPOLATION>linear< / INTERPOLATION>
	//	<INITIAL_VALUE>0.001< / INITIAL_VALUE>
	//	<FINAL_VALUE>0.0001< / FINAL_VALUE>
	//	<EVALUATION_EPISODE_VALUE>0.0< / EVALUATION_EPISODE_VALUE>
	//</ALPHA>
}

double CInterpolatedValue::get()
{
	double progress;
	bool eval = g_pExperiment->isEvaluationEpisode();

	if (!eval)
	{
		if (m_timeReference == experiment)
			progress = g_pExperiment->m_expProgress.getExperimentProgress();
		else
			progress = g_pExperiment->m_expProgress.getEpisodeProgress();

		switch (m_interpolation)
		{
		case linear:
			return m_startValue + progress*(m_endValue - m_startValue);
		case quadratic:
			return m_startValue + (1. - pow(1 - progress, 2.0))*(m_endValue - m_startValue);
		}
		assert(0);
		return 0.0;
	}
	else return m_evaluationValue;
}

INumericValue* XMLParameters::getNumericHandler(tinyxml2::XMLElement* pParameters)
{
	if (!pParameters->FirstChildElement())
		return new CConstantValue(pParameters);
	else return new CInterpolatedValue(pParameters);

	return 0;
}

int XMLParameters::countChildren(tinyxml2::XMLElement* pElement, const char* name)
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