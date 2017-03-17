#pragma once
class CConfigNode;
#include <memory>
#include "parameters.h"

class CNumericValue
{
public:
	virtual double get() = 0;

	static std::shared_ptr<CNumericValue> getInstance(CConfigNode* pConfigNode);
};

class CConstantValue : public CNumericValue
{
	DOUBLE_PARAM m_value;
public:
	CConstantValue(CConfigNode* pConfigNode);
	CConstantValue(double value);
	double get() { return m_value.get(); }
};


class CSimpleEpisodeLinearSchedule : public CNumericValue
{
	DOUBLE_PARAM m_startValue;
	DOUBLE_PARAM m_endValue;
public:
	CSimpleEpisodeLinearSchedule(CConfigNode* pParameters);
	CSimpleEpisodeLinearSchedule(double startValue, double endValue);
	double get();
};


class CInterpolatedValue : public CNumericValue
{
	DOUBLE_PARAM m_startOffset; //normalized offset to start
	DOUBLE_PARAM m_preOffsetValue; //value before the start of the schedule
	DOUBLE_PARAM m_startValue;
	DOUBLE_PARAM m_endValue;
	DOUBLE_PARAM m_evaluationValue;
	ENUM_PARAM<Interpolation> m_interpolation;
	ENUM_PARAM<TimeReference> m_timeReference;
public:
	CInterpolatedValue(CConfigNode* pParameters);
	CInterpolatedValue(double startOffset, double preOffsetValue, double startValue, double endValue, double evaluationValue
		, Interpolation interpolation, TimeReference timeReference);
	double get();
};


class CBhatnagarSchedule : public CNumericValue
{
	DOUBLE_PARAM m_alpha_0; //alpha_0
	DOUBLE_PARAM m_alpha_c; //alpha_c
	DOUBLE_PARAM m_t_exp; // exp
	DOUBLE_PARAM m_evaluationValue; //value returned during evaluation episodes
	ENUM_PARAM<TimeReference> m_timeReference;
public:
	//alpha_t= alpha_0*alpha_c / (alpha_c+t^{exp})
	CBhatnagarSchedule(CConfigNode* pParameters);
	CBhatnagarSchedule(double alpha_0, double alpha_c, double t_exp, double evaluationValue, TimeReference timeReference);
	double get();
};
