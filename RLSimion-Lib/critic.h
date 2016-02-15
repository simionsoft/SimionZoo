#pragma once

class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;

class tinyxml2::XMLElement;


class CCritic
{
	virtual void loadVFunction(const char* filename) = 0;
	virtual void saveVFunction(const char* filename) = 0;

public:
	CCritic(){}
	virtual ~CCritic(){}

	virtual double updateValue(CState *s, CAction *a, CState *s_p, double r, double rho) = 0;

	static CCritic *getInstance(tinyxml2::XMLElement* pParameters);
};

