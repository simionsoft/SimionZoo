#pragma once

#include "critic.h"
#include "parameterized-object.h"

class CLinearVFA;
class CParameters;
class CETraces;
class CFeatureList;
class CParameter;

class CVFACritic : public CCritic, public CParamObject
{
protected:
	CLinearVFA* m_pVFA; //value function approximator

	void loadVFunction(const char* filename);
	void saveVFunction(const char* filename);
public:
	CVFACritic(CParameters* pParameters);
	virtual ~CVFACritic();

	virtual double updateValue(CState *s, CAction *a, CState *s_p, double r, double rho) = 0;

	static CVFACritic* getInstance(CParameters* pParameters);
};

class CTDLambdaCritic : public CVFACritic
{
	CETraces* m_z; //traces
	CFeatureList* m_aux;

public:
	CTDLambdaCritic(CParameters *pParameters);
	~CTDLambdaCritic();

	double updateValue(CState *s, CAction *a, CState *s_p, double r, double rho);
};

class CTrueOnlineTDLambdaCritic : public CVFACritic
{
	//True Online TD(lambda)
	//Harm van Seijen, Richard Sutton
	//Proceedings of the 31st International Conference on Machine learning


	CETraces* m_e; //traces
	CFeatureList* m_aux;
	double m_v_s;

public:
	CTrueOnlineTDLambdaCritic(CParameters *pParameters);
	~CTrueOnlineTDLambdaCritic();

	double updateValue(CState *s, CAction *a, CState *s_p, double r, double rho);

};

class CTDCLambdaCritic : public CVFACritic
{
	CETraces* m_z; //traces
	CFeatureList* m_s_features;
	CFeatureList* m_s_p_features;
	CFeatureList* m_omega;
	CFeatureList* m_a;
	CFeatureList* m_b;

	CParameter *m_pAlpha;

public:
	CTDCLambdaCritic(CParameters *pParameters);
	~CTDCLambdaCritic();

	double updateValue(CState *s, CAction *a, CState *s_p, double r, double rho);
};
