#pragma once
#include "parameterized-object.h"

class CLinearVFA;
class CGaussianNoise;
class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;

class CSingleOutputVFAPolicy : public CParamObject
{

protected:
	CLinearVFA *m_pVFA;
	CGaussianNoise *m_pExpNoise;
public:
	CSingleOutputVFAPolicy(CParameters* pParameters);
	~CSingleOutputVFAPolicy();

	void selectAction(CState *s, CAction *a);

	CLinearVFA* getVFA(){ return m_pVFA; }
	CGaussianNoise* getExpNoise(){ return m_pExpNoise; }
};