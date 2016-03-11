#pragma once

#include "actor.h"
#include "parameterized-object.h"
class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;

class CFeatureList;
class CETraces;
class CDeterministicVFAPolicy;
class CParameters;
class INumericValue;

class CVFAPolicyLearner: public CParamObject
{
protected:
	CDeterministicVFAPolicy* m_pPolicy;

public:
	CVFAPolicyLearner(CParameters* pParameters);
	~CVFAPolicyLearner();

	virtual void updatePolicy(const CState *s, const CAction *a,const CState *s_p, double r, double td)= 0;

	CDeterministicVFAPolicy* getPolicy(){ return m_pPolicy; }

	static CVFAPolicyLearner* getInstance(CParameters* pParameters);
};


class CVFAActor: public CActor, public CParamObject
{	
	char* m_loadFile = 0;
	char* m_saveFile = 0;
protected:

	int m_numOutputs;
	CVFAPolicyLearner **m_pPolicyLearners;
public:
	CVFAActor(CParameters* pParameters);
	virtual ~CVFAActor();

	void selectAction(const CState *s, CAction *a);

	void updatePolicy(const CState *s, const CAction *a, const CState *s_p, double r, double td);

//	double getProbability(CState* s, CAction* a);

	void savePolicy(const char* pFilename);
	void loadPolicy(const char* pFilename);
};





class CCACLALearner : public CVFAPolicyLearner
{
	CFeatureList *m_pStateFeatures;
	CETraces *m_e;
	INumericValue* m_pAlpha;
public:

	CCACLALearner(CParameters *pParameters);
	~CCACLALearner();

	void updatePolicy(const CState *s, const CAction *a, const CState *s_p, double r, double td);
};

class CRegularPolicyGradientLearner :public CVFAPolicyLearner
{
	CFeatureList *m_pStateFeatures;
	CETraces *m_e;
	INumericValue* m_pAlpha;
public:

	CRegularPolicyGradientLearner(CParameters *pParameters);
	~CRegularPolicyGradientLearner();

	void updatePolicy(const CState *s, const CAction *a, const CState *s_p, double r, double td);
};

class CIncrementalNaturalActor : public CVFAPolicyLearner
{
	//"Model-free Reinforcement Learning with Continuous Action in Practice"
	//Thomas Degris, Patrick M. Pilarski, Richard S. Sutton
	//2012 American Control Conference
	CETraces *m_e;
	CFeatureList *m_grad_u;
	CFeatureList *m_s_features;
	CFeatureList *m_w;
	INumericValue* m_pGamma;
	INumericValue* m_pAlphaU;
	INumericValue* m_pAlphaV;
public:

	CIncrementalNaturalActor(CParameters *pParameters);
	~CIncrementalNaturalActor();

	void updatePolicy(const CState *s, const CAction *a, const CState *s_p, double r, double td);
};
