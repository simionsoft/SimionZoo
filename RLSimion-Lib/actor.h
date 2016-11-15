#pragma once

class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;

class CConfigNode;
class CPolicyLearner;
#include "delayed-load.h"
#include <vector>
class CController;

class CActor: public CDeferredLoad
{
	CController* m_pInitController;
protected:
	std::vector<CPolicyLearner*> m_policyLearners;

public:
	CActor(CConfigNode* pParameters);
	virtual ~CActor();

	virtual void selectAction(const CState *s, CAction *a);

	virtual void updatePolicy(const CState *s, const CAction *a, const CState *s_p, double r, double td);

	//CDeferredLoad
	virtual void deferredLoadStep();
};
