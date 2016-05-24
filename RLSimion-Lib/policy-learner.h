#pragma once

#include "parameterized-object.h"
class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;

class CFeatureList;
class CETraces;
class CPolicy;
class CParameters;
class CNumericValue;

class CPolicyLearner: public CParamObject
{
protected:
	CPolicy* m_pPolicy;

public:
	CPolicyLearner(CParameters* pParameters);
	virtual ~CPolicyLearner();

	virtual void updatePolicy(const CState *s, const CAction *a,const CState *s_p, double r, double td)= 0;

	CPolicy* getPolicy(){ return m_pPolicy; }

	static CPolicyLearner* getInstance(CParameters* pParameters);
};





class CCACLALearner : public CPolicyLearner
{
	CFeatureList *m_pStateFeatures;
	CETraces *m_e;
	CNumericValue* m_pAlpha;
public:

	CCACLALearner(CParameters *pParameters);
	virtual ~CCACLALearner();

	void updatePolicy(const CState *s, const CAction *a, const CState *s_p, double r, double td);
};

class CRegularPolicyGradientLearner :public CPolicyLearner
{
	CFeatureList *m_pStateFeatures;
	CETraces *m_e;
	CNumericValue* m_pAlpha;
public:

	CRegularPolicyGradientLearner(CParameters *pParameters);
	virtual ~CRegularPolicyGradientLearner();

	void updatePolicy(const CState *s, const CAction *a, const CState *s_p, double r, double td);
};
