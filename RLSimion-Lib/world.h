#pragma once

class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;
class CParameters;
class CDynamicModel;
class CReward;

class CWorld
{
	CDynamicModel* m_pDynamicModel;
	int m_simulationSteps;
	static double m_dt;
	static double m_t;
	static double m_step_start_t;
	CReward* m_pReward;
	double m_lastReward;
	double m_avgReward;
	double m_avgRewardGain= 0.9;
public:
	static double getDT();
	static double getT();
	static double getStepStartT();

	CWorld(CParameters* pParameters);
	~CWorld();

	CState *getStateInstance(); // pointer to a new CState. has to be deleted
	CAction *getActionInstance();

	CState *getStateDescriptor(); // pointer to the state descriptor. don't delete
	CAction *getActionDescriptor();

	void reset(CState *s);

	//this function returns the reward of the tuple <s,a,s_p>
	double executeAction(CState *s,CAction *a,CState *s_p);

	double getAvgReward();
	double getLastReward();
	CReward *getReward();

};

class CDynamicModel
{
protected:
	CState *m_pStateDescriptor;
	CAction *m_pActionDescriptor;
public:
	virtual ~CDynamicModel();
	virtual void reset(CState *s)= 0;
	virtual void executeAction(CState *s,CAction *a,double dt)= 0;

	CState* getStateDescriptor(){return m_pStateDescriptor;}
	CAction* getActionDescriptor(){return m_pActionDescriptor;}
};
