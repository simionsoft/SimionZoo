#pragma once

#include "actor.h"
#include "parameterized-object.h"

class CETraces;


class CSingleOutputPolicyLearner : public CParamObject
{

protected:
	CLinearVFA *m_pVFA;
	CGaussianNoise *m_pExpNoise;
	CFeatureList *m_pStateFeatures;

public:
	CSingleOutputPolicyLearner(CParameters* pParameters);
	~CSingleOutputPolicyLearner();

	void selectAction(CState *s, CAction *a);

	virtual void updatePolicy(CState *s, CAction *a, CState *s_p, double r, double td)= 0;

	CLinearVFA* getVFA(){ return m_pVFA; }
	CGaussianNoise* getExpNoise(){ return m_pExpNoise; }

	static CSingleOutputPolicyLearner* getInstance(CParameters* pParameters);
};


class CVFAActor: public CActor, public CParamObject
{
protected:
	int m_numOutputs;
	CSingleOutputPolicyLearner **m_pPolicyLearners;
public:
	CVFAActor(CParameters* pParameters);
	virtual ~CVFAActor();

	void selectAction(CState *s, CAction *a);

	void updatePolicy(CState *s, CAction *a, CState *s_p, double r, double td);

	double getProbability(CState* s, CAction* a);

	void savePolicy(const char* pFilename);
	void loadPolicy(const char* pFilename);
};





class CCACLAActor : public CSingleOutputPolicyLearner
{
	CETraces *m_e;
public:

	CCACLAActor(CParameters *pParameters);
	~CCACLAActor();

	void updatePolicy(CState *s, CAction *a, CState *s_p, double r, double td);
};

class CRegularActor :public CSingleOutputPolicyLearner
{
	CETraces *m_e;
public:

	CRegularActor(CParameters *pParameters);
	~CRegularActor();

	void updatePolicy(CState *s, CAction *a, CState *s_p, double r, double td);
};

class CIncrementalNaturalActor : public CSingleOutputPolicyLearner
{
	//"Model-free Reinforcement Learning with Continuous Action in Practice"
	//Thomas Degris, Patrick M. Pilarski, Richard S. Sutton
	//2012 American Control Conference
	CETraces *m_e;
	CFeatureList *m_grad_u;
public:

	CIncrementalNaturalActor(CParameters *pParameters);
	~CIncrementalNaturalActor();

	void updatePolicy(CState *s, CAction *a, CState *s_p, double r, double td);
};
