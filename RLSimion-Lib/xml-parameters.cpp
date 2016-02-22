#include "stdafx.h"
#include "xml-parameters.h"
#include "globals.h"
#include "experiment.h"

std::list<INumericValue*> XMLParameters::m_handlers;// = std::list<INumericValue*>();

class CConstantValue : public INumericValue
{
	double m_value;
public:
	CConstantValue(tinyxml2::XMLElement* pParameters)
	{
		//<ALPHA>0.1</ALPHA>
		m_value = atof(pParameters->GetText());
	}
	double getValue(){ return m_value; }
};

enum EnumInterpolation{linear, quadratic};
enum EnumTimeReference{ experiment,episode };

class CInterpolatedValue : public INumericValue
{
	double m_startOffset; //normalized offset to start
	double m_preOffsetValue; //value before the start of the schedule
	double m_startValue;
	double m_endValue;
	double m_evaluationValue;
	EnumInterpolation m_interpolation;
	EnumTimeReference m_timeReference;
public:
	CInterpolatedValue(tinyxml2::XMLElement* pParameters);
	double getValue();
};

CInterpolatedValue::CInterpolatedValue(tinyxml2::XMLElement* pParameters)
{
	tinyxml2::XMLElement* param;

	param = pParameters->FirstChildElement("Start-Offset");
	if (param)
		m_startOffset = XMLParameters::getConstDouble(param);
	else m_startOffset = 0.0;

	param = pParameters->FirstChildElement("Pre-Offset-Value");
	if (param)
		m_preOffsetValue = XMLParameters::getConstDouble(param);
	else m_preOffsetValue = 0.0;

	param = pParameters->FirstChildElement("Interpolation");
	assert(param);
	if (!strcmp("linear", param->GetText())) m_interpolation = linear;
	else if (!strcmp("quadratic", param->GetText())) m_interpolation = quadratic;
	else assert(0);

	param = pParameters->FirstChildElement("Time-reference");

	if (!param) m_timeReference = experiment;
	else
	{
		if (!strcmp("experiment", param->GetText())) m_timeReference = experiment;
		else if (!strcmp("episode", param->GetText())) m_timeReference = episode;
		else assert(0);
	}

	param = pParameters->FirstChildElement("Initial-Value");
	if (param) m_startValue = XMLParameters::getConstDouble(param);
	else assert(0);

	param = pParameters->FirstChildElement("End-Value");
	if (param) m_endValue = XMLParameters::getConstDouble(param);
	else assert(0);

	param = pParameters->FirstChildElement("Evaluation-Value");
	if (param) m_evaluationValue = XMLParameters::getConstDouble(param);
	else assert(0);
}

double CInterpolatedValue::getValue()
{
	double progress;

	//evalution episode?
	if (RLSimion::g_pExperiment->isEvaluationEpisode())
		return m_evaluationValue;
	//time reference
	switch (m_timeReference)
	{
	case experiment:
		progress = RLSimion::g_pExperiment->m_expProgress.getExperimentProgress();
		break;
	case episode:
	default:
		progress = RLSimion::g_pExperiment->m_expProgress.getEpisodeProgress();
	}

	if (m_startOffset != 0.0)
	{
		if (progress < m_startOffset) return m_preOffsetValue;

		progress = (progress - m_startOffset) / (1 - m_startOffset);
	}
	//interpolation
	switch (m_interpolation)
	{
	case linear:
		return m_startValue + (m_endValue - m_startValue)* progress;
	case quadratic:
	default:
		return m_startValue + (1. - pow(1 - progress, 2.0))*(m_endValue - m_startValue)* progress;
	}


}


int XMLParameters::countChildren(tinyxml2::XMLElement* pElement, const char* name)
{
	int count = 0;
	tinyxml2::XMLElement* p;
	
	if (name) p= pElement->FirstChildElement(name);
	else p = pElement->FirstChildElement();

	while (p != 0)
	{
		count++;

		if (name)
			p = p->NextSiblingElement(name);
		else p = p->NextSiblingElement();
	}
	return count;
}

INumericValue* XMLParameters::getNumericHandler(tinyxml2::XMLElement* pElement)
{
	INumericValue* pHandler;
	if (!pElement->FirstChildElement())
		pHandler= new CConstantValue(pElement);
	else if (pElement->FirstChildElement("decimal"))
		pHandler= new CConstantValue(pElement->FirstChildElement("decimal"));
	else pHandler= new CInterpolatedValue(pElement);

	m_handlers.push_front(pHandler);

	return pHandler;
}

void XMLParameters::freeHandlers()
{
	for (auto handler = m_handlers.begin(); handler != m_handlers.end(); handler++)
	{
		delete *handler;
	}
}

bool XMLParameters::getConstBoolean(tinyxml2::XMLElement* pParameters)
{
	assert(pParameters && pParameters->GetText());

	if (!strcmp(pParameters->GetText(), "true"))
		return true;
	if (!strcmp(pParameters->GetText(), "false"))
		return false;
	assert(0);
	return false;
}
int XMLParameters::getConstInteger(tinyxml2::XMLElement* pParameters)
{
	assert(pParameters && pParameters->GetText());
	return atoi(pParameters->GetText());
}
double XMLParameters::getConstDouble(tinyxml2::XMLElement* pParameters)
{
	assert(pParameters && pParameters->GetText());
	return atof(pParameters->GetText());
}

const char* XMLParameters::getConstString(tinyxml2::XMLElement* pParameters)
{
	assert(pParameters && pParameters->GetText());
	return pParameters->GetText();
}