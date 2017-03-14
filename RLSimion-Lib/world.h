#pragma once

class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;
typedef CNamedVarSet CReward;

class CDynamicModel;
class CConfigNode;
class CRewardFunction;

#include "../3rd-party/tinyxml2/tinyxml2.h"
#include "parameters.h"
#include "named-var-set.h"

class CDynamicModel
{
private:
	CDescriptor *m_pStateDescriptor;
	CDescriptor *m_pActionDescriptor;
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

	CDescriptor& getStateDescriptor();
	CDescriptor* getStateDescriptorPtr();
	CState* getStateInstance();
	CDescriptor& getActionDescriptor();
	CDescriptor* getActionDescriptorPtr();
	CAction* getActionInstance();

	double getConstant(const char* constantName);
	double getConstant(int i);
	const char* getConstantName(int i);
	int getNumConstants();


	static std::shared_ptr<CDynamicModel> getInstance(CConfigNode* pParameters);
};

class CWorld
{
	static CHILD_OBJECT_FACTORY<CDynamicModel> m_pDynamicModel;
	INT_PARAM m_numIntegrationSteps;
	DOUBLE_PARAM m_dt;

	//these times below are based on dt, that is, simulated time, not real time
	double m_episodeSimTime; // simulated time since the episode started
	double m_totalSimTime; // simulated time since the experiment started
	double m_stepStartSimTime; // the simulated time when last step started

public:
	double getDT();
	double getEpisodeSimTime();
	double getTotalSimTime();
	double getStepStartSimTime();
	static CDynamicModel* getDynamicModel(){ return m_pDynamicModel.ptr(); }

	CWorld(CConfigNode* pConfigNode);
	CWorld() = default;
	virtual ~CWorld();

	void reset(CState *s);

	//this function returns the reward of the tuple <s,a,s_p> and whether the resultant state is a failure state or not
	double executeAction(CState *s,CAction *a,CState *s_p);

	CReward *getRewardVector();
};


