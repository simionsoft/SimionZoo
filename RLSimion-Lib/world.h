#pragma once

class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;
typedef CNamedVarSet CReward;

class CDynamicModel;
class CConfigNode;
class CRewardFunction;
#include "../tinyxml2/tinyxml2.h"
#include "parameters.h"
#include <map>
#include <memory>

class CDynamicModel
{
private:
	CState *m_pStateDescriptor;
	CAction *m_pActionDescriptor;
	std::map<const char*, double> m_pConstants;
protected:
	CRewardFunction* m_pRewardFunction;
	int addStateVariable(const char* name, const char* units, double min, double max);
	int addActionVariable(const char* name, const char* units, double min, double max);
	void addConstant(const char* name, double value);
public:
	CDynamicModel();
	virtual ~CDynamicModel();

	virtual void reset(CState *s) = 0;
	virtual void executeAction(CState *s, const CAction *a, double dt) = 0;

	double getReward(const CState *s, const CAction *a, const CState *s_p);
	CReward* getRewardVector();

	CState* getStateDescriptor();
	CState* getStateInstance();
	CAction* getActionDescriptor();
	CAction* getActionInstance();

	double getConstant(const char* constantName);

	static std::shared_ptr<CDynamicModel> getInstance(CConfigNode* pParameters);
};

class CWorld
{
	static CHILD_OBJECT_FACTORY<CDynamicModel> m_pDynamicModel;
	INT_PARAM m_numIntegrationSteps;
	DOUBLE_PARAM m_dt;
	double m_t;
	double m_step_start_t;

public:
	double getDT();
	double getT();
	double getStepStartT();
	static CDynamicModel* getDynamicModel(){ return m_pDynamicModel.ptr(); }

	CWorld(CConfigNode* pConfigNode);
	virtual ~CWorld();

	void reset(CState *s);

	//this function returns the reward of the tuple <s,a,s_p> and whether the resultant state is a failure state or not
	double executeAction(CState *s,CAction *a,CState *s_p);

	CReward *getRewardVector();
};


