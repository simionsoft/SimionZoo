#pragma once

class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;
class CParameters;
class CParameter;
class CFeatureList;

class CFeatureVFA;
class CGaussianNoise;

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

	virtual double selectAction(CState *s,CAction *a)= 0;

	virtual void updatePolicy(CState *s,CAction *a,CState *s_p,double r,double td)= 0;

	static CActor *getActorInstance(CParameters* pParameters);
	static CActor *getControllerInstance(CParameters* pParameters);
};



class CCACLAActor:public CActor
{
	bool m_bSave;
	char m_saveFilename[1024];
	CFeatureList *m_pStateFeatures;
	CAction *m_pOutput;

	double getProbability(CAction* a);
	void savePolicy(const char* pFilename);
	void loadPolicy(const char* pFilename);
	CFeatureVFA **m_pPolicy;
public:
	CParameter **m_pAlpha;
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