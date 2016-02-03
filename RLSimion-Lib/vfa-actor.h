#pragma once

#include "actor.h"
#include "parameterized-object.h"
class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;

class CETraces;
class CSingleOutputVFAPolicy;


class CSingleOutputVFAPolicyLearner: public CParamObject
{
protected:
	CSingleOutputVFAPolicy* m_pPolicy;
public:
	CSingleOutputVFAPolicyLearner(CParameters* pParameters);
	~CSingleOutputVFAPolicyLearner();

	virtual void updatePolicy(CState *s, CAction *a, CState *s_p, double r, double td)= 0;

	CSingleOutputVFAPolicy* getPolicy(){ return m_pPolicy; }

	static CSingleOutputVFAPolicyLearner* getInstance(CParameters* pParameters);
};


class CVFAActor: public CActor, public CParamObject
{
protected:
	int m_numOutputs;
	CSingleOutputVFAPolicyLearner **m_pPolicyLearners;
public:
	CVFAActor(CParameters* pParameters);
	virtual ~CVFAActor();

	void selectAction(CState *s, CAction *a);

	void updatePolicy(CState *s, CAction *a, CState *s_p, double r, double td);

	double getProbability(CState* s, CAction* a);

	void savePolicy(const char* pFilename);
	void loadPolicy(const char* pFilename);
};





class CCACLALearner : public CSingleOutputVFAPolicyLearner
{
	CFeatureList *m_pStateFeatures;
	CETraces *m_e;
public:

	CCACLALearner(CParameters *pParameters);
	~CCACLALearner();

	void updatePolicy(CState *s, CAction *a, CState *s_p, double r, double td);
};

class CRegularPolicyGradientLearner :public CSingleOutputVFAPolicyLearner
{
	CFeatureList *m_pStateFeatures;
	CETraces *m_e;
public:

	CRegularPolicyGradientLearner(CParameters *pParameters);
	~CRegularPolicyGradientLearner();

	void updatePolicy(CState *s, CAction *a, CState *s_p, double r, double td);
};
//
//class CIncrementalNaturalActor : public CSingleOutputPolicyLearner
//{
//	//"Model-free Reinforcement Learning with Continuous Action in Practice"
//	//Thomas Degris, Patrick M. Pilarski, Richard S. Sutton
//	//2012 American Control Conference
//	CETraces *m_e;
//	CFeatureList *m_grad_u;
//	CFeatureList *m_s_features;
//public:
//
//	CIncrementalNaturalActor(CParameters *pParameters);
//	~CIncrementalNaturalActor();
//
//	void updatePolicy(CState *s, CAction *a, CState *s_p, double r, double td);
//};
