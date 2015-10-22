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
protected:
	int m_numOutputs;
public:
	CActor(){}
	virtual ~CActor(){};

	virtual double selectAction(CState *s,CAction *a)= 0;

	virtual void update(CState *s,CAction *a,CState *s_p,double r,double td)= 0;

	static CActor *getInstance(char* configFile);
};



class CCACLAActor:public CActor
{
	bool m_bSave;
	char m_saveFilename[1024];
	CFeatureList *m_pStateFeatures;

	void save(char* pFilename);
	void load(char* pFilename);
	double getProbability(CAction* a);

public:
	double **m_pAlpha;
	CFeatureVFA **m_pPolicy;
	CGaussianNoise **m_pExpNoise;

	CCACLAActor(CParameters *pParameters);
	~CCACLAActor();

	double selectAction(CState *s,CAction *a);

	void update(CState *s,CAction *a,CState *s_p,double r,double td);


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

	void update(CState *s,CAction *a,CState *s_p,double r,double td);

	double getLastActionProbability();
};*/