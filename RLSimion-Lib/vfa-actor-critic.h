#include "actor-critic.h"
#include "parameterized-object.h"

class CLinearVFA;
class CSingleOutputVFAPolicy;
class CETraces;

class CVFAActorCritic: public CActorCritic, public CParamObject
{
protected:
	//value function: V
	CLinearVFA *m_pVFunction;

	//policies: \pi_i (i=1..n)
	int m_numOutputs;
	CSingleOutputVFAPolicy **m_pPolicies;

	void loadVFunction(const char* filename);
	void saveVFunction(const char* filename);
	void savePolicy(const char* pFilename);
	void loadPolicy(const char* pFilename);
public:

	CVFAActorCritic(CParameters* pParameters);
	virtual ~CVFAActorCritic();


	virtual double updateValue(CState *s, CAction *a, CState *s_p, double r, double rho)= 0;

	virtual void selectAction(CState *s, CAction *a) = 0;

	virtual void updatePolicy(CState *s, CAction *a, CState *s_p, double r, double td)= 0;

	virtual double getProbability(CState* s, CAction* a) = 0;

	static CVFAActorCritic *getInstance(CParameters* pParameters);
};

class CIncrementalNaturalActorCritic: public CVFAActorCritic
{
	//"Model-free Reinforcement Learning with Continuous Action in Practice"
	//Thomas Degris, Patrick M. Pilarski, Richard S. Sutton
	//2012 American Control Conference
	
	CFeatureList *m_grad_u;
	CFeatureList *m_s_features;
	CFeatureList *m_s_p_features;
	CETraces** m_e_u;
	CFeatureList **m_w;
	CETraces* m_e_v;
	double m_avg_r;
public:
	CIncrementalNaturalActorCritic(CParameters* pParameters);
	virtual ~CIncrementalNaturalActorCritic();


	virtual double updateValue(CState *s, CAction *a, CState *s_p, double r, double rho);

	virtual void selectAction(CState *s, CAction *a);

	virtual void updatePolicy(CState *s, CAction *a, CState *s_p, double r, double td);

	virtual double getProbability(CState* s, CAction* a);
};