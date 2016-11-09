#pragma once

class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;
typedef CNamedVarSet CReward;

class CDynamicModel;
class CConfigNode;
class CRewardFunction;
#include "../tinyxml2/tinyxml2.h"

class CWorld
{
	static CDynamicModel* m_pDynamicModel;
	int m_numIntegrationSteps;
	double m_dt;
	double m_t;
	double m_step_start_t;

public:
	double getDT();
	double getT();
	double getStepStartT();
	static CDynamicModel* getDynamicModel(){ return m_pDynamicModel; }
	static void setDynamicModel(CDynamicModel* model){ m_pDynamicModel = model; }

	CWorld(CConfigNode* pParameters);
	virtual ~CWorld();

	void reset(CState *s);

	//this function returns the reward of the tuple <s,a,s_p> and whether the resultant state is a failure state or not
	double executeAction(CState *s,CAction *a,CState *s_p);

	CReward *getRewardVector();
};

class CDynamicModel
{
private:
	CState *m_pStateDescriptor;
	CAction *m_pActionDescriptor;
	CConfigNode *m_pConstants;
	tinyxml2::XMLDocument* m_pWorldConfigXMLDoc;
protected:
	CRewardFunction* m_pRewardFunction;
public:
	CDynamicModel(const char* pWorldDefinitionFile);
	virtual ~CDynamicModel();

	virtual void reset(CState *s)= 0;
	virtual void executeAction(CState *s, const CAction *a,double dt)= 0;
	
	double getReward(const CState *s, const CAction *a, const CState *s_p);
	CReward* getRewardVector();

	CState* getStateDescriptor();
	CState* getStateInstance();
	CAction* getActionDescriptor();
	CAction* getActionInstance();

	double getConstant(const char* constantName);

	static CDynamicModel* getInstance(CConfigNode* pParameters);
};
