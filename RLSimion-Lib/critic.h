#pragma once
#include "parameters.h"

class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;

class CConfigNode;
class CLinearStateVFA;

class ICritic
{
public:
	virtual double update(const CState *s, const CAction *a, const CState *s_p, double r, double rho = 1.0) = 0;

	static std::shared_ptr<ICritic> getInstance(CConfigNode* pParameters);
};

class CVLearnerCritic: public ICritic
{

protected:
	CHILD_OBJECT<CLinearStateVFA> m_pVFunction; //value function approximator

public:
	CVLearnerCritic(CConfigNode* pParameters);
	virtual ~CVLearnerCritic();

	virtual double update(const CState *s, const CAction *a, const CState *s_p, double r, double rho= 1.0) = 0;

};

