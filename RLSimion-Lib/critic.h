#pragma once

class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;
class CParameters;
class CFeatureVFA;
class CFeatureList;

class CCritic
{
public:
	CCritic(){}
	virtual ~CCritic(){}

	virtual double updateValue(CState *s,CAction *a,CState *s_p,double r, double rho)= 0;

	static CCritic *getInstance(char* configFile);

	virtual void load(char* filename){}
	virtual void save(char* filename){}
};

class CTDLambdaCritic: public CCritic
{
	char m_saveFilename[1024];

	CFeatureVFA* m_pVFA; //value function approximator
	CFeatureList* m_z; //traces
	CFeatureList* m_aux;

	double *m_pAlpha;
	double m_gamma;
	double m_lambda;
public:
	CTDLambdaCritic(CParameters *pParameters);
	~CTDLambdaCritic();

	double updateValue(CState *s, CAction *a, CState *s_p, double r, double rho);

	void load(char* filename);
	void save(char* filename);
};

class CTrueOnlineTDLambdaCritic: public CCritic
{
	//True Online TD(lambda)
	//Harm van Seijen, Richard Sutton
	//Proceedings of the 31st International Conference on Machine learning
	char m_saveFilename[1024];

	CFeatureVFA* m_pVFA; //value function approximator
	CFeatureList* m_e; //traces
	CFeatureList* m_aux;
	double m_v_s;

	double *m_pAlpha;
	double m_gamma;
	double m_lambda;
public:
	CTrueOnlineTDLambdaCritic(CParameters *pParameters);
	~CTrueOnlineTDLambdaCritic();

	double updateValue(CState *s, CAction *a, CState *s_p, double r, double rho);

	void load(char* filename);
	void save(char* filename);
};

class CTDCLambdaCritic : public CCritic
{
	char m_saveFilename[1024];

	CFeatureVFA* m_pVFA; //value function approximator
	CFeatureList* m_z; //traces
	CFeatureList* m_aux;
	CFeatureList* m_s_features;
	CFeatureList* m_s_p_features;
	CFeatureList* m_w;
	CFeatureList* m_a;

	double *m_pAlpha;
	double m_gamma;
	double m_lambda;
	double m_beta;
public:
	CTDCLambdaCritic(CParameters *pParameters);
	~CTDCLambdaCritic();

	double updateValue(CState *s, CAction *a, CState *s_p, double r, double rho);

	void load(char* filename);
	void save(char* filename);
};

