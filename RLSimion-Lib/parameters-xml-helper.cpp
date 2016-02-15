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
	double getValue(){ return m_value; }
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
	double getValue();
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
