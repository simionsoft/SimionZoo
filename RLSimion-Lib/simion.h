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
	virtual void update(const CState *s, const CAction *a, const CState *s_p, double r) = 0;

	virtual void selectAction(const CState *s, CAction *a) = 0;

	static std::shared_ptr<CSimion> getInstance(CConfigNode* pParameters);
};