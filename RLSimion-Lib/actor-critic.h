#pragma once
#include <vector>
#include "simion.h"
#include "parameterized-object.h"
class CParameters;
class CActor;
class CCritic;
class CPolicy;
class CETraces;
class CNumericValue;
class CFeatureList;
class CLinearStateVFA;

class CActorCritic: public CSimion, public CParamObject
{
	double m_td;
	CActor *m_pActor;
	CCritic *m_pCritic;
public:
	CActorCritic(CParameters* pParameters);
	virtual ~CActorCritic();

	virtual void selectAction(const CState *s, CAction *a);

	virtual void updateValue(const CState *s, const CAction *a, const CState *s_p, double r);

	virtual void updatePolicy(const CState *s, const CAction *a, const CState *s_p, double r);

	CActorCritic* getInstance(CParameters* pParameters);
};


class CIncrementalNaturalActorCritic : public CSimion
{
	//"Model-free Reinforcement Learning with Continuous Action in Practice"
	//Thomas Degris, Patrick M. Pilarski, Richard S. Sutton
	//2012 American Control Conference
	double m_td;

	CLinearStateVFA* m_pVFunction;
	int m_numPolicies;
	std::vector<CPolicy*> m_policies;

	double m_avg_r; 
	CETraces *m_e_u;
	CETraces* m_e_v;
	CFeatureList *m_grad_u;
	CFeatureList *m_s_features;
	CFeatureList *m_s_p_features;
	CFeatureList **m_w;
	CNumericValue* m_pGamma;
	CNumericValue* m_pAlphaU;
	CNumericValue* m_pAlphaV;
	CNumericValue *m_pAlphaR;

public:

	CIncrementalNaturalActorCritic(CParameters *pParameters);
	virtual ~CIncrementalNaturalActorCritic();

	virtual void selectAction(const CState *s, CAction *a);

	void updatePolicy(const CState *s, const CAction *a, const CState *s_p, double r);

	void updateValue(const CState *s, const CAction *a, const CState *s_p, double r);
};
