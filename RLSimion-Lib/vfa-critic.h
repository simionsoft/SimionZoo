#pragma once

#include "critic.h"
#include "parameterized-object.h"

class CLinearVFA;
class CETraces;
class CFeatureList;
class tinyxml2::XMLElement;
class INumericValue;


//Interface class

class CVFACritic : public CCritic, public CParamObject
{
protected:
	CLinearVFA* m_pVFA; //value function approximator

	void loadVFunction(const char* filename);
	void saveVFunction(const char* filename);
public:
	CVFACritic(tinyxml2::XMLElement* pParameters);
	virtual ~CVFACritic();

	virtual double updateValue(CState *s, CAction *a, CState *s_p, double r, double rho) = 0;

	static CVFACritic* getInstance(tinyxml2::XMLElement* pParameters);
};

//VFACritic implementations

class CTDLambdaCritic : public CVFACritic
{
	CETraces* m_z; //traces
	CFeatureList* m_aux;
	INumericValue* m_pAlpha;
	INumericValue* m_pGamma;
public:
	CTDLambdaCritic(tinyxml2::XMLElement *pParameters);
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
	INumericValue* m_pAlpha;
	INumericValue* m_pGamma;
public:
	CTrueOnlineTDLambdaCritic(tinyxml2::XMLElement *pParameters);
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

	INumericValue* m_pAlpha;
	INumericValue* m_pGamma;
	INumericValue* m_pBeta;
	

public:
	CTDCLambdaCritic(tinyxml2::XMLElement *pParameters);
	~CTDCLambdaCritic();

	double updateValue(CState *s, CAction *a, CState *s_p, double r, double rho);
};


class CIncrementalNaturalCritic : public CVFACritic
{
	CFeatureList *m_s_features;
	CFeatureList *m_s_p_features;

	CETraces* m_e_v;
	double m_avg_r;

	INumericValue *m_pAlphaV, *m_pAlphaR, *m_pGamma;
	
public:
	CIncrementalNaturalCritic(tinyxml2::XMLElement *pParameters);
	~CIncrementalNaturalCritic();

	double updateValue(CState *s, CAction *a, CState *s_p, double r, double rho);
};
