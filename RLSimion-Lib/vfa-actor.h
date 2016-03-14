#pragma once

#include "parameterized-object.h"
class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;

class CFeatureList;
class CETraces;
class CDeterministicPolicy;
class CParameters;
class CNumericValue;

class CPolicyLearner: public CParamObject
{
protected:
	CDeterministicPolicy* m_pPolicy;

public:
	CPolicyLearner(CParameters* pParameters);
	~CPolicyLearner();

	virtual void updatePolicy(const CState *s, const CAction *a,const CState *s_p, double r, double td)= 0;

	CDeterministicPolicy* getPolicy(){ return m_pPolicy; }

	static CPolicyLearner* getInstance(CParameters* pParameters);
};





class CCACLALearner : public CPolicyLearner
{
	CFeatureList *m_pStateFeatures;
	CETraces *m_e;
	CNumericValue* m_pAlpha;
public:

	CCACLALearner(CParameters *pParameters);
	~CCACLALearner();

	void updatePolicy(const CState *s, const CAction *a, const CState *s_p, double r, double td);
};

class CRegularPolicyGradientLearner :public CPolicyLearner
{
	CFeatureList *m_pStateFeatures;
	CETraces *m_e;
	CNumericValue* m_pAlpha;
public:

	CRegularPolicyGradientLearner(CParameters *pParameters);
	~CRegularPolicyGradientLearner();

	void updatePolicy(const CState *s, const CAction *a, const CState *s_p, double r, double td);
};
