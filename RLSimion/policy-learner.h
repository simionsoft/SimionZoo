#pragma once
#include "parameters.h"
class NamedVarSet;
typedef NamedVarSet State;
typedef NamedVarSet Action;

class FeatureList;
class ETraces;
class Policy;
class ConfigNode;
class NumericValue;

class PolicyLearner
{
protected:
	CHILD_OBJECT_FACTORY<Policy> m_pPolicy;

public:
	PolicyLearner(ConfigNode* pParameters);
	virtual ~PolicyLearner();

	virtual void update(const State *s, const Action *a,const State *s_p, double r, double td)= 0;

	Policy* getPolicy(){ return m_pPolicy.ptr(); }

	static std::shared_ptr<PolicyLearner> getInstance(ConfigNode* pParameters);
};





class CACLALearner : public PolicyLearner
{
	FeatureList *m_pStateFeatures;
	CHILD_OBJECT_FACTORY<NumericValue> m_pAlpha;
public:

	CACLALearner(ConfigNode *pParameters);
	virtual ~CACLALearner();

	void update(const State *s, const Action *a, const State *s_p, double r, double td);
};

class RegularPolicyGradientLearner :public PolicyLearner
{
	FeatureList *m_pStateFeatures;
	CHILD_OBJECT_FACTORY<NumericValue> m_pAlpha;
public:

	RegularPolicyGradientLearner(ConfigNode *pParameters);
	virtual ~RegularPolicyGradientLearner();

	void update(const State *s, const Action *a, const State *s_p, double r, double td);
};
