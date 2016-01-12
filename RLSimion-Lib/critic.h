#pragma once

class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;
class CParameters;
class CFeatureVFA;
class CFeatureList;
class CParameters;
class CParameter;

class CCritic
{
	static CCritic* m_pCritic;
public:
	CCritic(){}
	virtual ~CCritic(){}

	virtual double updateValue(CState *s, CAction *a, CState *s_p, double r, double rho) = 0;

	static CCritic *getCriticInstance(CParameters* pParameters);

	virtual void loadVFunction(const char* filename)= 0;
	virtual void saveVFunction(const char* filename)= 0;
};

class CVFACritic: public CCritic
{
protected:
	char m_saveFilename[1024];
	CFeatureVFA* m_pVFA; //value function approximator
public:
	CVFACritic(CParameters* pParameters);
	virtual ~CVFACritic();

	virtual double updateValue(CState *s, CAction *a, CState *s_p, double r, double rho) = 0;

	void loadVFunction(const char* filename);
	void saveVFunction(const char* filename);
};

class CTDLambdaCritic: public CVFACritic
{
	CFeatureList* m_z; //traces
	CFeatureList* m_aux;

	CParameter *m_pAlpha;
	double m_gamma;
	double m_lambda;
public:
	CTDLambdaCritic(CParameters *pParameters);
	~CTDLambdaCritic();

	double updateValue(CState *s, CAction *a, CState *s_p, double r, double rho);
};

class CTrueOnlineTDLambdaCritic: public CVFACritic
{
	//True Online TD(lambda)
	//Harm van Seijen, Richard Sutton
	//Proceedings of the 31st International Conference on Machine learning
	char m_saveFilename[1024];

	CFeatureList* m_e; //traces
	CFeatureList* m_aux;
	double m_v_s;

	CParameter *m_pAlpha;
	double m_gamma;
	double m_lambda;
public:
	CTrueOnlineTDLambdaCritic(CParameters *pParameters);
	~CTrueOnlineTDLambdaCritic();

	double updateValue(CState *s, CAction *a, CState *s_p, double r, double rho);

};

class CTDCLambdaCritic : public CVFACritic
{
	char m_saveFilename[1024];

	CFeatureList* m_z; //traces
	CFeatureList* m_s_features;
	CFeatureList* m_s_p_features;
	CFeatureList* m_omega;
	CFeatureList* m_a;
	CFeatureList* m_b;

	CParameter *m_pAlpha;
	double m_gamma;
	double m_lambda;
	double m_beta;
public:
	CTDCLambdaCritic(CParameters *pParameters);
	~CTDCLambdaCritic();

	double updateValue(CState *s, CAction *a, CState *s_p, double r, double rho);
};

