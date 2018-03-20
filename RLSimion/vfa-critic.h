#pragma once

#include "critic.h"
#include "parameters.h"

class LinearStateVFA;
class ETraces;
class FeatureList;
class ConfigNode;
class NumericValue;



//VLearnerCritic implementations

class TDLambdaCritic : public VLearnerCritic
{
	CHILD_OBJECT<ETraces> m_z; //traces
	FeatureList* m_aux;
	CHILD_OBJECT_FACTORY<NumericValue> m_pAlpha;

public:
	TDLambdaCritic(ConfigNode *pParameters);
	virtual ~TDLambdaCritic();

	double update(const State *s, const Action *a, const State *s_p, double r, double rho = 1.0);
};

class TrueOnlineTDLambdaCritic : public VLearnerCritic
{
	//True Online TD(lambda)
	//Harm van Seijen, Richard Sutton
	//Proceedings of the 31st International Conference on Machine learning

	CHILD_OBJECT<ETraces> m_e; //traces
	FeatureList* m_aux;
	double m_v_s;
	CHILD_OBJECT_FACTORY<NumericValue> m_pAlpha;
public:
	TrueOnlineTDLambdaCritic(ConfigNode *pParameters);
	virtual ~TrueOnlineTDLambdaCritic();

	double update(const State *s, const Action *a, const State *s_p, double r, double rho = 1.0);

};

class TDCLambdaCritic : public VLearnerCritic
{
	CHILD_OBJECT<ETraces> m_z; //traces
	FeatureList* m_s_features;
	FeatureList* m_s_p_features;
	FeatureList* m_omega;
	FeatureList* m_a;
	FeatureList* m_b;

	CHILD_OBJECT_FACTORY<NumericValue> m_pAlpha;
	CHILD_OBJECT_FACTORY<NumericValue> m_pBeta;
	

public:
	TDCLambdaCritic(ConfigNode *pParameters);
	virtual ~TDCLambdaCritic();

	double update(const State *s, const Action *a, const State *s_p, double r, double rho= 1.0);
};


