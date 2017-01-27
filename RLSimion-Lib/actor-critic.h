#pragma once
#include <vector>
#include "simion.h"
#include "parameters.h"
class CConfigNode;
class CActor;
class CCritic;
class CPolicy;
class CETraces;
class CNumericValue;
class CFeatureList;
class CLinearStateVFA;

class CActorCritic: public CSimion
{
	double m_td;
	CHILD_OBJECT<CActor> m_pActor;
	CHILD_OBJECT_FACTORY<CCritic> m_pCritic;
public:
	CActorCritic(CConfigNode* pParameters);
	virtual ~CActorCritic() = default;

	virtual void selectAction(const CState *s, CAction *a);

	virtual void updateValue(const CState *s, const CAction *a, const CState *s_p, double r);

	virtual void updatePolicy(const CState *s, const CAction *a, const CState *s_p, double r);
};


class CIncrementalNaturalActorCritic : public CSimion
{
	//"Model-free Reinforcement Learning with Continuous Action in Practice"
	//Thomas Degris, Patrick M. Pilarski, Richard S. Sutton
	//2012 American Control Conference
	double m_td;

	CHILD_OBJECT<CLinearStateVFA> m_pVFunction;
	MULTI_VALUE_FACTORY<CPolicy> m_policies;

	double m_avg_r; 
	CHILD_OBJECT<CETraces> m_e_u;
	CHILD_OBJECT<CETraces> m_e_v;
	CFeatureList *m_grad_u;
	CFeatureList *m_s_features;
	CFeatureList *m_s_p_features;
	CFeatureList **m_w;
	CHILD_OBJECT_FACTORY<CNumericValue> m_pGamma;
	CHILD_OBJECT_FACTORY<CNumericValue> m_pAlphaU;
	CHILD_OBJECT_FACTORY<CNumericValue> m_pAlphaV;
	CHILD_OBJECT_FACTORY<CNumericValue> m_pAlphaR;

public:

	CIncrementalNaturalActorCritic(CConfigNode *pParameters);
	virtual ~CIncrementalNaturalActorCritic();

	virtual void selectAction(const CState *s, CAction *a);

	void updatePolicy(const CState *s, const CAction *a, const CState *s_p, double r);

	void updateValue(const CState *s, const CAction *a, const CState *s_p, double r);
};
