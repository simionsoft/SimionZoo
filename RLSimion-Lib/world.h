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
	//static to be able to access it from the CRewardComponent constructor without resorting to the yet unassigned
	//global pointer RLSimion::g_pWorld
	static CDynamicModel* m_pDynamicModel;
	int m_simulationSteps;
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

	CWorld(CParameters* pParameters);
	~CWorld();

	//CState *getStateInstance(); // pointer to a new CState. has to be deleted
	//CAction *getActionInstance();

	//CState *getStateDescriptor(); // pointer to the state descriptor. don't delete
	//CAction *getActionDescriptor();

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

	CState* getStateDescriptor(){return m_pStateDescriptor;}
	CAction* getActionDescriptor(){return m_pActionDescriptor;}
	double* getConstant(const char* constantName);

	static CDynamicModel* getInstance(CParameters* pParameters);
};
