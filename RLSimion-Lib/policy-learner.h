#pragma once
#include "parameters.h"
class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;

class CFeatureList;
class CETraces;
class CPolicy;
class CConfigNode;
class CNumericValue;

class CPolicyLearner
{
protected:
	CHILD_OBJECT_FACTORY<CPolicy> m_pPolicy;

public:
	CPolicyLearner(CConfigNode* pParameters);
	virtual ~CPolicyLearner();

	virtual void update(const CState *s, const CAction *a,const CState *s_p, double r, double td)= 0;

	CPolicy* getPolicy(){ return m_pPolicy.ptr(); }

	static std::shared_ptr<CPolicyLearner> getInstance(CConfigNode* pParameters);
};





class CCACLALearner : public CPolicyLearner
{
	CFeatureList *m_pStateFeatures;
	CHILD_OBJECT<CETraces> m_e;
	CHILD_OBJECT_FACTORY<CNumericValue> m_pAlpha;
public:

	CCACLALearner(CConfigNode *pParameters);
	virtual ~CCACLALearner();

	void update(const CState *s, const CAction *a, const CState *s_p, double r, double td);
};

class CRegularPolicyGradientLearner :public CPolicyLearner
{
	CFeatureList *m_pStateFeatures;
	CHILD_OBJECT<CETraces> m_e;
	CHILD_OBJECT_FACTORY<CNumericValue> m_pAlpha;
public:

	CRegularPolicyGradientLearner(CConfigNode *pParameters);
	virtual ~CRegularPolicyGradientLearner();

	void update(const CState *s, const CAction *a, const CState *s_p, double r, double td);
};
