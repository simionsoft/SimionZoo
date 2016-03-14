#pragma once

#include "critic.h"
#include "parameterized-object.h"

class CLinearStateVFA;
class CETraces;
class CFeatureList;
class CParameters;
class CNumericValue;



//CCritic implementations

class CTDLambdaCritic : public CCritic
{
	CETraces* m_z; //traces
	CFeatureList* m_aux;
	CNumericValue* m_pAlpha;
	CNumericValue* m_pGamma;
public:
	CTDLambdaCritic(CParameters *pParameters);
	~CTDLambdaCritic();

	double updateValue(const CState *s, const CAction *a, const CState *s_p, double r);
};

class CTrueOnlineTDLambdaCritic : public CCritic
{
	//True Online TD(lambda)
	//Harm van Seijen, Richard Sutton
	//Proceedings of the 31st International Conference on Machine learning

	CETraces* m_e; //traces
	CFeatureList* m_aux;
	double m_v_s;
	CNumericValue* m_pAlpha;
	CNumericValue* m_pGamma;
public:
	CTrueOnlineTDLambdaCritic(CParameters *pParameters);
	~CTrueOnlineTDLambdaCritic();

	double updateValue(const CState *s, const CAction *a, const CState *s_p, double r);

};

class CTDCLambdaCritic : public CCritic
{
	CETraces* m_z; //traces
	CFeatureList* m_s_features;
	CFeatureList* m_s_p_features;
	CFeatureList* m_omega;
	CFeatureList* m_a;
	CFeatureList* m_b;

	CNumericValue* m_pAlpha;
	CNumericValue* m_pGamma;
	CNumericValue* m_pBeta;
	

public:
	CTDCLambdaCritic(CParameters *pParameters);
	~CTDCLambdaCritic();

	double updateValue(const CState *s, const CAction *a, const CState *s_p, double r);
};


