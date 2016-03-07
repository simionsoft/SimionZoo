#pragma once
#include "parameterized-object.h"

class CLinearStateVFA;
class CNoise;

class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;

class CParameters;


class CSingleOutputVFAPolicy : public CParamObject
{

protected:
	CLinearStateVFA *m_pVFA;
	CNoise *m_pExpNoise;
	int m_outputActionIndex;
	const char* m_outputAction;
public:
	CSingleOutputVFAPolicy(CParameters* pParameters);
	~CSingleOutputVFAPolicy();

	void selectAction(const CState *s, CAction *a);

	CLinearStateVFA* getVFA(){ return m_pVFA; }
	CNoise* getExpNoise(){ return m_pExpNoise; }
	const char* getOutputAction(){ return m_outputAction; }
	int getOutputActionIndex(){ return m_outputActionIndex; }

	//returns the factor by which the state features have to be multiplied to get the policy gradient
	void getGradient(const CState* s, const CAction* a, CFeatureList* pOutGradient);
};