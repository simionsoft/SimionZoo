#pragma once

class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;
typedef CNamedVarSet CReward;

class CDynamicModel;
class CRewardFunction;
class CParameters;

class CWorld
{
	static CDynamicModel* m_pDynamicModel;
	int m_numIntegrationSteps;
	double m_dt;
	double m_t;
	double m_step_start_t;
	CRewardFunction* m_pRewardFunction;
	double m_scalarReward;

public:
	double getDT();
	double getT();
	double getStepStartT();
	static CDynamicModel* getDynamicModel(){ return m_pDynamicModel; }
	static void setDynamicModel(CDynamicModel* model){ m_pDynamicModel = model; }

	CWorld();
	~CWorld();

	void init(CParameters* pParameters);

	void reset(CState *s);

	//this function returns the reward of the tuple <s,a,s_p>
	double executeAction(CState *s,CAction *a,CState *s_p);

	double getAvgReward();
	double getLastReward();

	CReward *getReward();
	double *getScalarReward(){ return &m_scalarReward; }
};

class CDynamicModel
{
private:
	CState *m_pStateDescriptor;
	CAction *m_pActionDescriptor;
	CParameters *m_pConstants;

public:
	CDynamicModel(const char* pWorldDefinitionFile);
	virtual ~CDynamicModel();

	virtual void reset(CState *s)= 0;
	virtual void executeAction(CState *s,CAction *a,double dt)= 0;

	CState* getStateDescriptor();
	CAction* getActionDescriptor();
	CState* getStateInstance();
	CAction* getActionInstance();
	double* getConstant(const char* constantName);

	static CDynamicModel* getInstance(CParameters* pParameters);
};
