#include "stdafx.h"
#include "parameters.h"
#include "globals.h"
#include "experiment.h"
#include "logger.h"

#ifdef _DEBUG
#pragma comment (lib,"../Debug/tinyxml2.lib")
#else
#pragma comment (lib,"../Release/tinyxml2.lib")
#endif

std::list<INumericValue*> CParameters::m_handlers;

class CConstantValue : public INumericValue
{
	double m_value;
public:
	CConstantValue(CParameters* pParameters)
	{
		//<ALPHA>0.1</ALPHA>
		m_value = atof(pParameters->getConstString());
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
	CInterpolatedValue(CParameters* pParameters);
	double getValue();
};

CInterpolatedValue::CInterpolatedValue(CParameters* pParameters)
{
	m_startOffset = pParameters->getConstDouble("Start-Offset", 0.0);
	m_preOffsetValue = pParameters->getConstDouble("Pre-Offset-Value", 0.0);

	const char* strValue = pParameters->getConstString("Interpolation","linear");

	if (!strcmp("linear", strValue)) m_interpolation = linear;
	else if (!strcmp("quadratic", strValue)) m_interpolation = quadratic;
	else m_interpolation = linear;


	strValue = pParameters->getConstString("Time-reference","experiment");
	if (!strcmp("experiment", strValue)) m_timeReference = experiment;
	else if (!strcmp("episode", strValue)) m_timeReference = episode;
	else	m_timeReference = experiment;

	m_startValue = pParameters->getConstDouble( "Initial-Value", 0.0);
	m_endValue = pParameters->getConstDouble( "End-Value", 1.0);
	m_evaluationValue = pParameters->getConstDouble( "Evaluation-Value", 0.0);
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
		progress = RLSimion::g_pExperiment->getExperimentProgress();
		break;
	case episode:
	default:
		progress = RLSimion::g_pExperiment->getEpisodeProgress();
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
	CBhatnagarSchedule(CParameters* pParameters);
	double getValue();
};


CBhatnagarSchedule::CBhatnagarSchedule(CParameters* pParameters)
{
	const char* strValue;

	strValue = pParameters->getConstString("Time-reference");
	if (!strcmp("experiment", strValue)) m_timeReference = experiment;
	else if (!strcmp("episode", strValue)) m_timeReference = episode;
	else m_timeReference = episode;

	m_alpha_0 = pParameters->getConstDouble( "Alpha-0");
	m_alpha_c = pParameters->getConstDouble("Alpha-c");
	m_t_exp = pParameters->getConstDouble("Time-Exponent", 1.0);
	m_evaluationValue = pParameters->getConstDouble("Evaluation-Value");
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
		t = RLSimion::g_pExperiment->getStep() 
			+ (RLSimion::g_pExperiment->getEpisodeIndex()-1) * RLSimion::g_pExperiment->getNumSteps();
		break;
	case episode:
	default:
		t = RLSimion::g_pExperiment->getStep();
	}
	
	return m_alpha_0*m_alpha_c / (m_alpha_c + pow(t, m_t_exp));
}

CParameters* CParameterFile::loadFile(const char* fileName, const char* nodeName)
{
	LoadFile(fileName);
	if (Error()) return 0;


	return (CParameters*) (this->FirstChildElement(nodeName));
	
}

const char* CParameterFile::getError()
{
	return ErrorName();
}


int CParameters::countChildren(const char* name)
{
	int count = 0;
	CParameters* p;
	
	if (name) p= getChild(name);
	else p = getChild();

	while (p != 0)
	{
		count++;

		if (name)
			p = p->getNextChild(name);
		else p = p->getNextChild();
	}
	return count;
}

INumericValue* CParameters::getNumericHandler(const char* paramName)
{
	INumericValue* pHandler;
	CParameters* pParameter = getChild(paramName);
	
	if (pParameter->getChild("Linear-Schedule"))
		pHandler = new CInterpolatedValue((CParameters*)pParameter->getChild("Linear-Schedule"));
	else if (pParameter->getChild("Bhatnagar-Schedule"))
		pHandler = new CBhatnagarSchedule((CParameters*)pParameter->getChild("Bhatnagar-Schedule"));
	else pHandler = new CConstantValue(pParameter);

	m_handlers.push_front(pHandler);

	return pHandler;
}

void CParameters::freeHandlers()
{
	for (auto handler = m_handlers.begin(); handler != m_handlers.end(); handler++)
	{
		delete *handler;
	}
}


bool CParameters::getConstBoolean(const char* paramName, bool defaultValue)
{
	tinyxml2::XMLElement* pParameter;

	pParameter = getChild(paramName);
	if (pParameter)
	{
		if (!strcmp(pParameter->GetText(), "true"))
			return true;
		if (!strcmp(pParameter->GetText(), "false"))
			return false;
	}
	char msg[128];
	sprintf_s(msg, 128, "Parameter %s/%s not found. Using default value %b", getName(), paramName, defaultValue);
	CLogger::logMessage(Warning, msg);

	return defaultValue;
}

int CParameters::getConstInteger(const char* paramName, int defaultValue)
{
	tinyxml2::XMLElement* pParameter;

	pParameter = getChild(paramName);
	if (pParameter)
	{
		return atoi(pParameter->GetText());
	}
	char msg[128];
	sprintf_s(msg, 128, "Parameter %s/%s not found. Using default value %d", getName(), paramName, defaultValue);
	CLogger::logMessage(Warning, msg);
	return defaultValue;
}

double CParameters::getConstDouble(const char* paramName, double defaultValue)
{
	CParameters* pParameter;

	pParameter = getChild(paramName);
	if (pParameter)
	{
		return atof(pParameter->GetText());
	}
	char msg[128];
	sprintf_s(msg, 128, "Parameter %s/%s not found. Using default value %f", getName(), paramName, defaultValue);
	CLogger::logMessage(Warning, msg);

	return defaultValue;
}

const char* CParameters::getConstString(const char* paramName, const char* defaultValue)
{
	CParameters* pParameter;

	if (paramName)
	{
		pParameter = getChild(paramName);
		if (pParameter)
		{
			return pParameter->GetText();
		}
	}
	else
	if (GetText()) return GetText();
	char msg[128];
	sprintf_s(msg, 128, "Parameter %s/%s not found. Using default value %s", getName(), paramName, defaultValue);
	CLogger::logMessage(Warning, msg);

	return defaultValue;
}

CParameters* CParameters::getChild(const char* paramName)
{
	tinyxml2::XMLElement* child = FirstChildElement(paramName);
	return static_cast<CParameters*> (child);
}

CParameters* CParameters::getNextChild(const char* paramName)
{
	tinyxml2::XMLElement* child = NextSiblingElement(paramName);
	return static_cast<CParameters*> (child);
}

const char* CParameters::getName()
{
	return Name();
}

void CParameters::saveFile(const char* pFilename)
{
	SaveFile(pFilename, false);
}

void CParameters::saveFile(FILE* pFile)
{
	SaveFile(pFile, false);
}