#include "stdafx.h"
#include "parameters-numeric.h"
#include "globals.h"
#include "parameters.h"
#include "experiment.h"

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
	CONST_DOUBLE_VALUE(m_value, pParameters, "Value", 0.0);
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

	CONST_DOUBLE_VALUE(m_startOffset, pParameters, "Start-Offset", 0.0);
	CONST_DOUBLE_VALUE(m_preOffsetValue, pParameters, "Pre-Offset-Value", 0.0);

	ENUM_VALUE(m_interpolation, Interpolation, pParameters, "Interpolation", "linear");

	ENUM_VALUE(m_timeReference, TimeReference, pParameters, "Time-reference", "experiment");

	CONST_DOUBLE_VALUE(m_startValue, pParameters, "Initial-Value", 0.0);
	CONST_DOUBLE_VALUE(m_endValue, pParameters, "End-Value", 1.0);
	CONST_DOUBLE_VALUE(m_evaluationValue, pParameters, "Evaluation-Value", 0.0);
	END_CLASS();
}


double CInterpolatedValue::getValue()
{
	double progress;

	//evalution episode?
	if (RLSimion::g_pExperiment->isEvaluationEpisode())
		return m_evaluationValue;
	//time reference
	if (!strcmp(m_timeReference, "experiment"))
		progress = RLSimion::g_pExperiment->getExperimentProgress();
	else if (!strcmp(m_timeReference, "episode"))
		progress = RLSimion::g_pExperiment->getEpisodeProgress();
	/*else if (!strcmp(m_timeReference, "episode"))
	progress = RLSimion::g_pExperiment->get();*/
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
	ENUM_VALUE(m_timeReference, TimeReference, pParameters, "Time-reference", "linear");

	CONST_DOUBLE_VALUE(m_alpha_0, pParameters, "Alpha-0", 1.0);
	CONST_DOUBLE_VALUE(m_alpha_c, pParameters, "Alpha-c", 1.0);
	CONST_DOUBLE_VALUE(m_t_exp, pParameters, "Time-Exponent", 1.0);
	CONST_DOUBLE_VALUE(m_evaluationValue, pParameters, "Evaluation-Value", 0.0);
	END_CLASS();
}


double CBhatnagarSchedule::getValue()
{
	double t;

	//evalution episode?
	if (RLSimion::g_pExperiment->isEvaluationEpisode())
		return m_evaluationValue;
	//time reference
	if (!strcmp(m_timeReference, "experiment"))
		t = RLSimion::g_pExperiment->getStep()
		+ (RLSimion::g_pExperiment->getEpisodeIndex() - 1) * RLSimion::g_pExperiment->getNumSteps();
	else if (!strcmp(m_timeReference, "episode"))
		t = RLSimion::g_pExperiment->getStep();
	else assert(0);

	return m_alpha_0*m_alpha_c / (m_alpha_c + pow(t, m_t_exp));
}


CLASS_FACTORY(CNumericValue)
{
	CHOICE("Schedule");
	CHOICE_ELEMENT("Constant", CConstantValue);
	CHOICE_ELEMENT("Linear-Schedule", CInterpolatedValue);
	CHOICE_ELEMENT("Bhatnagar-Schedule", CBhatnagarSchedule);
	END_CHOICE();
	END_CLASS();

	return 0;
}
