#pragma once
class ConfigNode;
#include <memory>
#include "parameters.h"

class NumericValue
{
public:
	virtual double get() = 0;

	static std::shared_ptr<NumericValue> getInstance(ConfigNode* pConfigNode);
};

class ConstantValue : public NumericValue
{
	DOUBLE_PARAM m_value;
public:
	ConstantValue(ConfigNode* pConfigNode);
	ConstantValue(double value);
	double get() { return m_value.get(); }
};


class SimpleEpisodeLinearSchedule : public NumericValue
{
	DOUBLE_PARAM m_startValue;
	DOUBLE_PARAM m_endValue;
public:
	SimpleEpisodeLinearSchedule(ConfigNode* pParameters);
	SimpleEpisodeLinearSchedule(double startValue, double endValue);
	double get();
};


class InterpolatedValue : public NumericValue
{
	DOUBLE_PARAM m_startOffset; //normalized offset to start
	DOUBLE_PARAM m_endTime; //normalized end-time
	DOUBLE_PARAM m_preOffsetValue; //value before the start of the schedule
	DOUBLE_PARAM m_startValue;
	DOUBLE_PARAM m_endValue;
	DOUBLE_PARAM m_evaluationValue;
	ENUM_PARAM<Interpolation> m_interpolation;
	ENUM_PARAM<TimeReference> m_timeReference;
public:
	InterpolatedValue(ConfigNode* pParameters);
	InterpolatedValue(double startOffset, double endTime, double preOffsetValue, double startValue, double endValue, double evaluationValue
		, Interpolation interpolation, TimeReference timeReference);
	double get();
};


class BhatnagarSchedule : public NumericValue
{
	DOUBLE_PARAM m_alpha_0; //alpha_0
	DOUBLE_PARAM m_alpha_c; //alpha_c
	DOUBLE_PARAM m_t_exp; // exp
	DOUBLE_PARAM m_evaluationValue; //value returned during evaluation episodes
	ENUM_PARAM<TimeReference> m_timeReference;
public:
	//alpha_t= alpha_0*alpha_c / (alpha_c+t^{exp})
	BhatnagarSchedule(ConfigNode* pParameters);
	BhatnagarSchedule(double alpha_0, double alpha_c, double t_exp, double evaluationValue, TimeReference timeReference);
	double get();
};

class WireConnection : public NumericValue
{
	WIRE_CONNECTION m_wire;
public:
	WireConnection(ConfigNode* pParameters);
	WireConnection(const char* name);
	double get();
	void set(double value);
};