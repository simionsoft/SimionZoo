#pragma once

class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;
class CParameters;
class CFeatureList;

class CFeatureVFA;
class CGaussianNoise;

class CActor
{
	static CActor* m_pActor;
protected:
	int m_numOutputs;
	CFeatureVFA **m_pPolicy;

	void savePolicy(char* pFilename);
	void loadPolicy(char* pFilename);
public:
	CActor(){}
	virtual ~CActor(){};

	virtual double selectAction(CState *s,CAction *a)= 0;

	virtual void updatePolicy(CState *s,CAction *a,CState *s_p,double r,double td)= 0;

	static CActor *getActorInstance(CParameters* pParameters);
};



class CCACLAActor:public CActor
{
	bool m_bSave;
	char m_saveFilename[1024];
	CFeatureList *m_pStateFeatures;


	double getProbability(CAction* a);

public:
	double **m_pAlpha;
	CGaussianNoise **m_pExpNoise;

	CCACLAActor(CParameters *pParameters);
	~CCACLAActor();

	double selectAction(CState *s,CAction *a);

	void updatePolicy(CState *s,CAction *a,CState *s_p,double r,double td);
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