#pragma once

class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;

class CParameters;

class CSimion
{

public:
	virtual void updateValue(const CState *s, const CAction *a, const CState *s_p, double r) = 0;

	virtual void selectAction(const CState *s, CAction *a) = 0;

	virtual void updatePolicy(const CState *s, const CAction *a, const CState *s_p, double r)= 0;


	static CSimion* getInstance(CParameters* pParameters);
};