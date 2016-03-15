#pragma once

class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;

class CParameters;
class CPolicyLearner;
#include "parameterized-object.h"

class CActor: public CParamObject
{
	const char* m_loadFile = 0;
	const char* m_saveFile = 0;
	void savePolicy(const char* pFilename);
	void loadPolicy(const char* pFilename);

protected:
	CPolicyLearner **m_pPolicyLearners;
	int m_numOutputs;

public:
	CActor(CParameters* pParameters);
	virtual ~CActor();

	virtual void selectAction(const CState *s, CAction *a);

	virtual void updatePolicy(const CState *s, const CAction *a, const CState *s_p, double r, double td);



};
