#pragma once

class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;
class CParameters;
class CLinearVFA;
class CFeatureList;
class CParameters;
class CParameter;

#include "parameters.h"
#include "features.h"

class CCritic
{
public:
	CCritic(){}
	virtual ~CCritic(){}

	virtual double updateValue(CState *s, CAction *a, CState *s_p, double r, double rho) = 0;

	static CCritic *getInstance(CParameters* pParameters);

	virtual void loadVFunction(const char* filename)= 0;
	virtual void saveVFunction(const char* filename)= 0;
};

