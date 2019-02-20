#include "parameters-numeric.h"
#include "config.h"
#include "experiment.h"
#include "app.h"
#include <assert.h>
#include <math.h>

ConstantValue::ConstantValue(ConfigNode* pConfigNode)
{
	m_value = DOUBLE_PARAM(pConfigNode, "Value", "Constant value", 0.0);
}

ConstantValue::ConstantValue(double value)
{
	m_value.set(value);
}

SimpleEpisodeLinearSchedule::SimpleEpisodeLinearSchedule(ConfigNode* pConfigNode)
{
	m_startValue = DOUBLE_PARAM(pConfigNode, "Initial-Value", "Value at the beginning of the experiment",0.1);
	m_endValue = DOUBLE_PARAM(pConfigNode, "End-Value", "Value at the end of the experiment", 0.0);
}

SimpleEpisodeLinearSchedule::SimpleEpisodeLinearSchedule(double startValue, double endValue)
{
	m_startValue.set(startValue);
	m_endValue.set(endValue);
}

double SimpleEpisodeLinearSchedule::get()
{
	if (SimionApp::get()->pExperiment->isEvaluationEpisode()) return 0.0;

	double progress = SimionApp::get()->pExperiment->getTrainingProgress();

	return m_startValue.get() + (m_endValue.get() - m_startValue.get())* progress;
}


InterpolatedValue::InterpolatedValue(ConfigNode* pConfigNode)
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

InterpolatedValue::InterpolatedValue(double startOffset, double endTime, double preOffsetValue, double startValue, double endValue, double evaluationValue
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

double InterpolatedValue::get()
{
	double progress;

	//evalution episode?
	if (SimionApp::get()->pExperiment->isEvaluationEpisode())
		return m_evaluationValue.get();

	//time reference
	if (m_timeReference.get()==TimeReference::experiment)
		progress = SimionApp::get()->pExperiment->getTrainingProgress();
	else if (m_timeReference.get()==TimeReference::episode)
		progress = SimionApp::get()->pExperiment->getEpisodeProgress();

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




BhatnagarSchedule::BhatnagarSchedule(ConfigNode* pConfigNode)
{
	m_timeReference = ENUM_PARAM<TimeReference>(pConfigNode, "Time-reference", "The time reference", TimeReference::episode);
	m_alpha_0 = DOUBLE_PARAM(pConfigNode, "Alpha-0", "Alpha-0 parameter in Bhatnagar's schedule", 1.0);
	m_alpha_c = DOUBLE_PARAM(pConfigNode, "Alpha-c", "Alpha-c parameter in Bhatnagar's schedule", 1.0);
	m_t_exp = DOUBLE_PARAM(pConfigNode, "Time-Exponent", "Time exponent in Bhatnagar's schedule", 1.0);
	m_evaluationValue= DOUBLE_PARAM(pConfigNode, "Evaluation-Value", "Output value during evaluation episodes", 0.0);
}

BhatnagarSchedule::BhatnagarSchedule(double alpha_0, double alpha_c, double t_exp
	, double evaluationValue, TimeReference timeReference)
{
	m_alpha_0.set(alpha_0);
	m_alpha_c.set(alpha_c);
	m_t_exp.set(t_exp);
	m_evaluationValue.set(evaluationValue);
	m_timeReference.set(timeReference);
}


double BhatnagarSchedule::get()
{
	double t;

	//evalution episode?
	if (SimionApp::get()->pExperiment->isEvaluationEpisode())
		return m_evaluationValue.get();
	//time reference
	if (m_timeReference.get()==TimeReference::experiment)
		t = SimionApp::get()->pExperiment->getStep()
		+ (SimionApp::get()->pExperiment->getEpisodeIndex() - 1) * SimionApp::get()->pExperiment->getNumSteps();
	else if (m_timeReference.get()==TimeReference::episode)
		t = SimionApp::get()->pExperiment->getStep();
	else assert(0);

	return m_alpha_0.get()*m_alpha_c.get() / (m_alpha_c.get() + pow(t, m_t_exp.get()));
}

WireConnection::WireConnection(ConfigNode* pParameters)
{
	m_wire = WIRE_CONNECTION(pParameters, "Wire", "Wire connection from which the value comes");
}

WireConnection::WireConnection(const char* name)
{
	m_wire = WIRE_CONNECTION (name);
}

double WireConnection::get()
{
	return m_wire.get();
}

void WireConnection::set(double value)
{
	m_wire.set(value);
}

std::shared_ptr<NumericValue> NumericValue::getInstance(ConfigNode* pConfigNode)
{
	return CHOICE<NumericValue>(pConfigNode, "Schedule", "Schedule-type",
	{
		{"Constant",CHOICE_ELEMENT_NEW<ConstantValue>},
		{"Linear-Schedule",CHOICE_ELEMENT_NEW<InterpolatedValue>},
		{"Simple-Linear-Decay",CHOICE_ELEMENT_NEW<SimpleEpisodeLinearSchedule>},
		{"Bhatnagar-Schedule",CHOICE_ELEMENT_NEW<BhatnagarSchedule>},
		{"Wire-Connection",CHOICE_ELEMENT_NEW<WireConnection>}
	});
}
