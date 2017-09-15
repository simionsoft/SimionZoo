#pragma once
#ifdef _WIN64
#include <vector>
#include "simion.h"
#include "critic.h"
#include "parameters.h"
#include "q-learners.h"
#include "deep-vfa-policy.h"



class CDQN : public CSimion
{
protected:
	MULTI_VALUE_FACTORY<CDeepPolicy> m_policies;
	CFeatureList* m_pStateOutFeatures;
	
public:
	~CDQN();
	CDQN(CConfigNode *pParameters);


	//selects an according to the learned policy pi(a|s)
	virtual double selectAction(const CState *s, CAction *a);

	//updates the critic and the actor
	virtual double update(const CState *s, const CAction *a, const CState *s_p, double r, double behaviorProb);
};
#endif
