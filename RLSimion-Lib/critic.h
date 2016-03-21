#pragma once

class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;

class CParameters;
class CLinearStateVFA;
#include "parameterized-object.h"

class CCritic: public CParamObject
{

protected:
	CLinearStateVFA* m_pVFunction; //value function approximator

public:
	CCritic(CParameters* pParameters);
	virtual ~CCritic();

	virtual double updateValue(const CState *s, const CAction *a, const CState *s_p, double r) = 0;

	static CCritic *getInstance(CParameters* pParameters);
};

