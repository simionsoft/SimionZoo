#pragma once

class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;

#include "parameters.h"
#include "delayed-load.h"
#include "policy-learner.h"
#include "controller.h"
#include <vector>

class CActor: public CDeferredLoad
{
	CHILD_OBJECT_FACTORY<CController> m_pInitController;
protected:
	MULTI_VALUE_FACTORY<CPolicyLearner> m_policyLearners;

public:
	CActor(CConfigNode* pConfigNode);
	CActor() = default;
	virtual ~CActor();

	virtual void selectAction(const CState *s, CAction *a);

	virtual void updatePolicy(const CState *s, const CAction *a, const CState *s_p, double r, double td);

	//CDeferredLoad
	virtual void deferredLoadStep();
};
