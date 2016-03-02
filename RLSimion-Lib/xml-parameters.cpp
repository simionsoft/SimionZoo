#include "stdafx.h"
#include "xml-parameters.h"
#include "globals.h"
#include "experiment.h"

std::list<INumericValue*> XMLUtils::m_handlers;// = std::list<INumericValue*>();

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
		m_startOffset = XMLUtils::getConstDouble(param);
	else m_startOffset = 0.0;

	param = pParameters->FirstChildElement("Pre-Offset-Value");
	if (param)
		m_preOffsetValue = XMLUtils::getConstDouble(param);
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
	if (param) m_startValue = XMLUtils::getConstDouble(param);
	else assert(0);

	param = pParameters->FirstChildElement("End-Value");
	if (param) m_endValue = XMLUtils::getConstDouble(param);
	else assert(0);

	param = pParameters->FirstChildElement("Evaluation-Value");
	if (param) m_evaluationValue = XMLUtils::getConstDouble(param);
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

class CBhatnagarSchedule : public INumericValue
{
	double m_alpha_0; //alpha_0
	double m_alpha_c; //alpha_c
	double m_t_exp; // exp
	double m_evaluationValue; //value returned during evaluation episodes
	EnumTimeReference m_timeReference;
public:
	//alpha_t= alpha_0*alpha_c / (alpha_c+t^{exp})
	CBhatnagarSchedule(tinyxml2::XMLElement* pParameters);
	double getValue();
};


CBhatnagarSchedule::CBhatnagarSchedule(tinyxml2::XMLElement* pParameters)
{
	tinyxml2::XMLElement* param;

	param = pParameters->FirstChildElement("Time-reference");

	if (!param) m_timeReference = experiment;
	else
	{
		if (!strcmp("experiment", param->GetText())) m_timeReference = experiment;
		else if (!strcmp("episode", param->GetText())) m_timeReference = episode;
		else assert(0);
	}

	param = pParameters->FirstChildElement("Alpha-0");
	if (param) m_alpha_0 = XMLUtils::getConstDouble(param);
	else assert(0);

	param = pParameters->FirstChildElement("Alpha-c");
	if (param) m_alpha_c = XMLUtils::getConstDouble(param);
	else assert(0);

	param = pParameters->FirstChildElement("Time-Exponent");
	if (param) m_t_exp = XMLUtils::getConstDouble(param);
	else assert(0);

	param = pParameters->FirstChildElement("Evaluation-Value");
	if (param) m_evaluationValue = XMLUtils::getConstDouble(param);
	else assert(0);
}


double CBhatnagarSchedule::getValue()
{
	double t;

	//evalution episode?
	if (RLSimion::g_pExperiment->isEvaluationEpisode())
		return m_evaluationValue;
	//time reference
	switch (m_timeReference)
	{
	case experiment:
		t = RLSimion::g_pExperiment->m_expProgress.getStep() 
			+ (RLSimion::g_pExperiment->m_expProgress.getEpisode()-1) * RLSimion::g_pExperiment->m_expProgress.getNumSteps();
		break;
	case episode:
	default:
		t = RLSimion::g_pExperiment->m_expProgress.getStep();
	}
	
	return m_alpha_0*m_alpha_c / (m_alpha_c + pow(t, m_t_exp));
}


int XMLUtils::countChildren(tinyxml2::XMLElement* pElement, const char* name)
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

INumericValue* XMLUtils::getNumericHandler(tinyxml2::XMLElement* pElement)
{
	INumericValue* pHandler;
	if (!pElement->FirstChildElement())
		pHandler= new CConstantValue(pElement);
	else if (pElement->FirstChildElement("Linear-Schedule"))
		pHandler = new CInterpolatedValue(pElement->FirstChildElement("Linear-Schedule"));
	else if (pElement->FirstChildElement("Bhatnagar-Schedule"))
		pHandler = new CBhatnagarSchedule(pElement->FirstChildElement("Bhatnagar-Schedule"));
	else assert(0);

	m_handlers.push_front(pHandler);

	return pHandler;
}

void XMLUtils::freeHandlers()
{
	for (auto handler = m_handlers.begin(); handler != m_handlers.end(); handler++)
	{
		delete *handler;
	}
}

bool XMLUtils::getConstBoolean(tinyxml2::XMLElement* pParameters)
{
	assert(pParameters && pParameters->GetText());

	if (!strcmp(pParameters->GetText(), "true"))
		return true;
	if (!strcmp(pParameters->GetText(), "false"))
		return false;
	assert(0);
	return false;
}
int XMLUtils::getConstInteger(tinyxml2::XMLElement* pParameters)
{
	assert(pParameters && pParameters->GetText());
	return atoi(pParameters->GetText());
}
double XMLUtils::getConstDouble(tinyxml2::XMLElement* pParameters)
{
	assert(pParameters && pParameters->GetText());
	return atof(pParameters->GetText());
}

const char* XMLUtils::getConstString(tinyxml2::XMLElement* pParameters)
{
	assert(pParameters && pParameters->GetText());
	return pParameters->GetText();
}