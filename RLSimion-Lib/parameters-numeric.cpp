#include "stdafx.h"
#include "parameters-numeric.h"
#include "globals.h"
#include "parameters.h"
#include "experiment.h"
#include "app.h"

//std::list<INumericValue*> INumericValue::m_handlers;

class CConstantValue : public CNumericValue
{
	double m_value;
public:
	CConstantValue(CParameters* pParameters);
	double getValue(){ return m_value; }
};

CLASS_CONSTRUCTOR(CConstantValue)
{
	//<ALPHA>0.1</ALPHA>
	CONST_DOUBLE_VALUE(m_value, "Value", 0.0,"Constant value");
	END_CLASS();
}



class CInterpolatedValue : public CNumericValue
{
	double m_startOffset; //normalized offset to start
	double m_preOffsetValue; //value before the start of the schedule
	double m_startValue;
	double m_endValue;
	double m_evaluationValue;
	const char* m_interpolation;
	const char* m_timeReference;
public:
	CInterpolatedValue(CParameters* pParameters);
	double getValue();
};

CLASS_CONSTRUCTOR(CInterpolatedValue)
{

	CONST_DOUBLE_VALUE(m_startOffset, "Start-Offset", 0.0,"Normalized time from which the schedule will begin [0...1]");
	CONST_DOUBLE_VALUE(m_preOffsetValue, "Pre-Offset-Value", 0.0,"Output value before the schedule begins");

	ENUM_VALUE(m_interpolation, Interpolation, "Interpolation", "linear","Interpolation type");

	ENUM_VALUE(m_timeReference, TimeReference, "Time-reference", "experiment","Time reference");

	CONST_DOUBLE_VALUE(m_startValue, "Initial-Value", 0.0,"Output value at the beginning of the schedule");
	CONST_DOUBLE_VALUE(m_endValue, "End-Value", 1.0,"Output value at the end of the schedule");
	CONST_DOUBLE_VALUE(m_evaluationValue, "Evaluation-Value", 0.0,"Output value during evaluation episodes");
	END_CLASS();
}


double CInterpolatedValue::getValue()
{
	double progress;

	//evalution episode?
	if (CApp::get()->Experiment.isEvaluationEpisode())
		return m_evaluationValue;
	//time reference
	if (!strcmp(m_timeReference, "experiment"))
		progress = CApp::get()->Experiment.getExperimentProgress();
	else if (!strcmp(m_timeReference, "episode"))
		progress = CApp::get()->Experiment.getEpisodeProgress();
	/*else if (!strcmp(m_timeReference, "episode"))
	progress = CApp::get()->Experiment.get();*/
	else assert(0);

	if (m_startOffset != 0.0)
	{
		if (progress < m_startOffset) return m_preOffsetValue;

		progress = (progress - m_startOffset) / (1 - m_startOffset);
	}
	//interpolation
	if (!strcmp(m_interpolation, "linear"))
		return m_startValue + (m_endValue - m_startValue)* progress;
	else if (!strcmp(m_interpolation, "quadratic"))
		return m_startValue + (1. - pow(1 - progress, 2.0))*(m_endValue - m_startValue)* progress;
	else if (!strcmp(m_interpolation, "cubic"))
		return m_startValue + (1. - pow(1 - progress, 3.0))*(m_endValue - m_startValue)* progress;
	else assert(0);

	return 0.0;
}

class CBhatnagarSchedule : public CNumericValue
{
	double m_alpha_0; //alpha_0
	double m_alpha_c; //alpha_c
	double m_t_exp; // exp
	double m_evaluationValue; //value returned during evaluation episodes
	const char* m_timeReference;
public:
	//alpha_t= alpha_0*alpha_c / (alpha_c+t^{exp})
	CBhatnagarSchedule(CParameters* pParameters);
	double getValue();
};


CLASS_CONSTRUCTOR(CBhatnagarSchedule)
{
	ENUM_VALUE(m_timeReference, TimeReference,  "Time-reference", "linear","Time reference");

	CONST_DOUBLE_VALUE(m_alpha_0,  "Alpha-0", 1.0,"Alpha-0 parameter in Bhatnagar's schedule");
	CONST_DOUBLE_VALUE(m_alpha_c, "Alpha-c", 1.0, "Alpha-c parameter in Bhatnagar's schedule");
	CONST_DOUBLE_VALUE(m_t_exp, "Time-Exponent", 1.0, "Time exponent in Bhatnagar's schedule");
	CONST_DOUBLE_VALUE(m_evaluationValue, "Evaluation-Value", 0.0,"Output value during evaluation episodes");
	END_CLASS();
}


double CBhatnagarSchedule::getValue()
{
	double t;

	//evalution episode?
	if (CApp::get()->Experiment.isEvaluationEpisode())
		return m_evaluationValue;
	//time reference
	if (!strcmp(m_timeReference, "experiment"))
		t = CApp::get()->Experiment.getStep()
		+ (CApp::get()->Experiment.getEpisodeIndex() - 1) * CApp::get()->Experiment.getNumSteps();
	else if (!strcmp(m_timeReference, "episode"))
		t = CApp::get()->Experiment.getStep();
	else assert(0);

	return m_alpha_0*m_alpha_c / (m_alpha_c + pow(t, m_t_exp));
}


CLASS_FACTORY_NEW_WINDOW(CNumericValue)
{
	CHOICE("Schedule","Schedule-type");
	CHOICE_ELEMENT("Constant", CConstantValue,"Constant value");
	CHOICE_ELEMENT("Linear-Schedule", CInterpolatedValue,"Linear schedule");
	CHOICE_ELEMENT("Bhatnagar-Schedule", CBhatnagarSchedule,"Bhatnagar's schedule");
	END_CHOICE();
	END_CLASS();

	return 0;
}
