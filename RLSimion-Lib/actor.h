#pragma once

class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;
class CParameters;
class CParameter;
class CFeatureList;

class CFeatureVFA;
class CGaussianNoise;

#include "experiment.h"

class CActor
{
	static CActor* m_pActor;
	static CActor* m_pController;

protected:
	int m_numOutputs;

	virtual void savePolicy(const char* pFilename){};
	virtual void loadPolicy(const char* pFilename){};
public:
	CActor(){}
	virtual ~CActor(){};

	virtual void selectAction(CState *s,CAction *a)= 0;

	virtual void updatePolicy(CState *s,CAction *a,CState *s_p,double r,double td)= 0;

	virtual double getProbability(CState* s, CAction* a){ return 1.0; }

	static CActor *getActorInstance(CParameters* pParameters);
	static CActor *getControllerInstance(CParameters* pParameters);
};

class CVFAGaussianNoiseActor : public CActor
{
protected:
	bool m_bSave;
	char m_saveFilename[1024];

	CFeatureVFA **m_pPolicy;	
	CParameter **m_pAlpha;
	CGaussianNoise **m_pExpNoise;

	CFeatureList *m_pStateFeatures;
	CAction *m_pOutput;
	CExperimentProgress m_outputTime;
public:
	CVFAGaussianNoiseActor(CParameters* pParameters);
	~CVFAGaussianNoiseActor();
	void savePolicy(const char* pFilename);
	void loadPolicy(const char* pFilename);

	double getProbability(CState* s, CAction* a);

	void selectAction(CState *s, CAction *a);
};


class CCACLAActor :public CVFAGaussianNoiseActor
{

public:

	CCACLAActor(CParameters *pParameters);
	~CCACLAActor();

	void updatePolicy(CState *s, CAction *a, CState *s_p, double r, double td);
};

/*
class CRegularGradientActor:public CActor
{
	bool m_bSave;
	char m_saveFilename[1024];
	CFeatureList *m_pStateFeatures;

	void save(char* pFilename);
	void load(char* pFilename);
public:
	double **m_pAlpha;
	CFeatureVFA **m_pPolicy;
	CGaussianNoise **m_pExpNoise;

	CRegularGradientActor(CParameters *pParameters);
	~CRegularGradientActor();

	double selectAction(CState *s,CAction *a);

	void updatePolicy(CState *s,CAction *a,CState *s_p,double r,double td);

	double getLastActionProbability();
};*/