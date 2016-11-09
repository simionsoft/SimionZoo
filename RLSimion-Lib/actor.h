#pragma once

class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;

class CParameters;
class CPolicyLearner;
#include "parameterized-object.h"
#include "delayed-load.h"
#include <vector>
class CController;

class CActor: public CParamObject, public CDeferredLoad
{
	CController* m_pInitController;
protected:
	std::vector<CPolicyLearner*> m_policyLearners;

public:
	CActor(CParameters* pParameters);
	virtual ~CActor();

	virtual void selectAction(const CState *s, CAction *a);

	virtual void updatePolicy(const CState *s, const CAction *a, const CState *s_p, double r, double td);

	//CDeferredLoad
	virtual void deferredLoadStep();
};
