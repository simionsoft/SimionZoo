#pragma once
#include "parameters.h"
class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;

class CConfigNode;

class CSimion
{

public:
	virtual ~CSimion(){};
	virtual void update(const CState *s, const CAction *a, const CState *s_p, double r, double probability) = 0;

	//selectAction sets output in a, and returns the probability under which the simion selected the action
	virtual double selectAction(const CState *s, CAction *a) = 0;

	static std::shared_ptr<CSimion> getInstance(CConfigNode* pParameters);
};