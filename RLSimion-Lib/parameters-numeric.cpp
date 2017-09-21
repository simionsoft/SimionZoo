#include "stdafx.h"
#include "parameters-numeric.h"
#include "config.h"
#include "experiment.h"
#include "app.h"


CConstantValue::CConstantValue(CConfigNode* pConfigNode)
{
	m_value = DOUBLE_PARAM(pConfigNode, "Value", "Constant value", 0.0);
}

CConstantValue::CConstantValue(double value)
{
	m_value.set(value);
}

CSimpleEpisodeLinearSchedule::CSimpleEpisodeLinearSchedule(CConfigNode* pConfigNode)
{
	m_startValue = DOUBLE_PARAM(pConfigNode, "Initial-Value", "Value at the beginning of the experiment",0.1);
	m_endValue = DOUBLE_PARAM(pConfigNode, "End-Value", "Value at the end of the experiment", 0.0);
}

CSimpleEpisodeLinearSchedule::CSimpleEpisodeLinearSchedule(double startValue, double endValue)
{
	m_startValue.set(startValue);
	m_endValue.set(endValue);
}

double CSimpleEpisodeLinearSchedule::get()
{
	if (CSimionApp::get()->pExperiment->isEvaluationEpisode()) return 0.0;

	double progress = CSimionApp::get()->pExperiment->getTrainingProgress();

	return m_startValue.get() + (m_endValue.get() - m_startValue.get())* progress;
}


CInterpolatedValue::CInterpolatedValue(CConfigNode* pConfigNode)
{
	m_startOffset = DOUBLE_PARAM(pConfigNode, "Start-Offset", "Normalized time from which the schedule will begin [0...1]", 0.0);
	m_endTime = DOUBLE_PARAM(pConfigNode, "End-Offset", "Normalized time at which the schedule will end and only return the End-Value [0...1]", 1.0);
	m_preOffsetValue = DOUBLE_PARAM(pConfigNode, "Pre-Offset-Value", "Output value before the schedule begins", 0.1);
	m_interpolation = ENUM_PARAM<Interpolation>(pConfigNode, "Interpolation", "Interpolation type", Interpolation::linear);
	m_timeReference = ENUM_PARAM<TimeReference>(pConfigNode, "Time-reference", "The time-reference type", TimeReference::experiment);
	m_startValue = DOUBLE_PARAM(pConfigNode,"Initial-Value", "Output value at the beginning of the schedule", 0.1);
	m_endValue = DOUBLE_PARAM(pConfigNode, "End-Value", "Output value at the end of the schedule", 0.0);
	m_evaluationValue = DOUBLE_PARAM(pConfigNode, "Evaluation-Value", "Output value during evaluation episodes", 0.0);
}

CInterpolatedValue::CInterpolatedValue(double startOffset, double endTime, double preOffsetValue, double startValue, double endValue, double evaluationValue
	, Interpolation interpolation, TimeReference timeReference)
{
	m_startOffset.set(startOffset);
	m_endTime.set(endTime);
	m_preOffsetValue.set(preOffsetValue);
	m_startValue.set(startValue);
	m_endValue.set(endValue);
	m_evaluationValue.set(evaluationValue);
	m_interpolation.set(interpolation);
	m_timeReference.set(timeReference);
}

double CInterpolatedValue::get()
{
	double progress;

	//evalution episode?
	if (CSimionApp::get()->pExperiment->isEvaluationEpisode())
		return m_evaluationValue.get();

	//time reference
	if (m_timeReference.get()==TimeReference::experiment)
		progress = CSimionApp::get()->pExperiment->getTrainingProgress();
	else if (m_timeReference.get()==TimeReference::episode)
		progress = CSimionApp::get()->pExperiment->getEpisodeProgress();

	if (m_startOffset.get() != 0.0)
	{
		if (progress < m_startOffset.get()) return m_preOffsetValue.get();

		progress = (progress - m_startOffset.get()) / (m_endTime.get() - m_startOffset.get());
	}
	//interpolation
	if (m_interpolation.get()==Interpolation::linear)
		return m_startValue.get() + (m_endValue.get() - m_startValue.get())* progress;

	else if (m_interpolation.get()==Interpolation::quadratic)
		return m_startValue.get()+ (1. - pow(1 - progress, 2.0))*(m_endValue.get() - m_startValue.get())* progress;

	else if (m_interpolation.get()==Interpolation::cubic)
		return m_startValue.get() + (1. - pow(1 - progress, 3.0))*(m_endValue.get() - m_startValue.get())* progress;
	else assert(0);

	return 0.0;
}




CBhatnagarSchedule::CBhatnagarSchedule(CConfigNode* pConfigNode)
{
	m_timeReference = ENUM_PARAM<TimeReference>(pConfigNode, "Time-reference", "The time reference", TimeReference::episode);
	m_alpha_0 = DOUBLE_PARAM(pConfigNode, "Alpha-0", "Alpha-0 parameter in Bhatnagar's schedule", 1.0);
	m_alpha_c = DOUBLE_PARAM(pConfigNode, "Alpha-c", "Alpha-c parameter in Bhatnagar's schedule", 1.0);
	m_t_exp = DOUBLE_PARAM(pConfigNode, "Time-Exponent", "Time exponent in Bhatnagar's schedule", 1.0);
	m_evaluationValue= DOUBLE_PARAM(pConfigNode, "Evaluation-Value", "Output value during evaluation episodes", 0.0);
}

CBhatnagarSchedule::CBhatnagarSchedule(double alpha_0, double alpha_c, double t_exp
	, double evaluationValue, TimeReference timeReference)
{
	m_alpha_0.set(alpha_0);
	m_alpha_c.set(alpha_c);
	m_t_exp.set(t_exp);
	m_evaluationValue.set(evaluationValue);
	m_timeReference.set(timeReference);
}


double CBhatnagarSchedule::get()
{
	double t;

	//evalution episode?
	if (CSimionApp::get()->pExperiment->isEvaluationEpisode())
		return m_evaluationValue.get();
	//time reference
	if (m_timeReference.get()==TimeReference::experiment)
		t = CSimionApp::get()->pExperiment->getStep()
		+ (CSimionApp::get()->pExperiment->getEpisodeIndex() - 1) * CSimionApp::get()->pExperiment->getNumSteps();
	else if (m_timeReference.get()==TimeReference::episode)
		t = CSimionApp::get()->pExperiment->getStep();
	else assert(0);

	return m_alpha_0.get()*m_alpha_c.get() / (m_alpha_c.get() + pow(t, m_t_exp.get()));
}


std::shared_ptr<CNumericValue> CNumericValue::getInstance(CConfigNode* pConfigNode)
{
	return CHOICE<CNumericValue>(pConfigNode, "Schedule", "Schedule-type",
	{
		{"Constant",CHOICE_ELEMENT_NEW<CConstantValue>},
		{"Linear-Schedule",CHOICE_ELEMENT_NEW<CInterpolatedValue>},
		{"Simple-Linear-Decay",CHOICE_ELEMENT_NEW<CSimpleEpisodeLinearSchedule>},
		{"Bhatnagar-Schedule",CHOICE_ELEMENT_NEW<CBhatnagarSchedule>}
	});

}
