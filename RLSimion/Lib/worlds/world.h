#pragma once

class NamedVarSet;
class NamedVarSet;
using State = NamedVarSet;
using Action = NamedVarSet;
using Reward = NamedVarSet;

class DynamicModel;
class ConfigNode;
class RewardFunction;

#include "../../../3rd-party/tinyxml2/tinyxml2.h"
#include "../parameters.h"
#include "../../Common/named-var-set.h"

struct cmp_str
{
	bool operator()(const char *a, const char *b) const
	{
		return std::strcmp(a, b) < 0;
	}
};


class DynamicModel
{
	Descriptor *m_pStateDescriptor;
	Descriptor *m_pActionDescriptor;
	std::map<const char*, double, cmp_str> m_pConstants;
protected:
	string m_name= string("");
	RewardFunction* m_pRewardFunction;
public:
	DynamicModel();
	virtual ~DynamicModel();

	string getWorldSceneFile() { return m_name + ".scene"; }

	size_t addStateVariable(const char* name, const char* units, double min, double max, bool bCircular= false);
	size_t addActionVariable(const char* name, const char* units, double min, double max, bool bCircular = false);
	void addConstant(const char* name, double value);

	const string getName() { return m_name; }

	virtual void reset(State *s) = 0;
	virtual void executeAction(State *s, const Action *a, double dt) = 0;

	double getReward(const State *s, const Action *a, const State *s_p);
	Reward* getRewardVector();
	Reward* getRewardInstance();

	Descriptor& getStateDescriptor();
	Descriptor* getStateDescriptorPtr();
	State* getStateInstance();
	Descriptor& getActionDescriptor();
	Descriptor* getActionDescriptorPtr();
	Action* getActionInstance();

	double getConstant(const char* constantName);
	double getConstant(int i);
	const char* getConstantName(int i);
	int getNumConstants();

	static std::shared_ptr<DynamicModel> getInstance(ConfigNode* pParameters);
};

class World
{
	static CHILD_OBJECT_FACTORY<DynamicModel> m_pDynamicModel;
	INT_PARAM m_numIntegrationSteps;
	DOUBLE_PARAM m_dt;

	//these times below are based on dt, that is, simulated time, not real time
	double m_episodeSimTime; // simulated time since the episode started
	double m_totalSimTime; // simulated time since the experiment started
	double m_stepStartSimTime; // the simulated time when last step started

	bool m_bFirstIntegrationStep = true; //is the current one the first integration step within a control step?
public:
	double getDT();
	double getEpisodeSimTime();
	double getTotalSimTime();
	double getStepStartSimTime();
	static DynamicModel* getDynamicModel(){ return m_pDynamicModel.ptr(); }
	bool bIsFirstIntegrationStep() { return m_bFirstIntegrationStep; }
	void setIsFirstIntegrationStep(bool bFirstIntegrationStep) { m_bFirstIntegrationStep = bFirstIntegrationStep; }

	void overrideDT(double dt) { m_dt.set(dt); }
	void overrideNumIntegrationSteps(int numIntegrationSteps) { m_numIntegrationSteps.set(numIntegrationSteps); }

	World(ConfigNode* pConfigNode);
	World() = default;
	virtual ~World();

	void reset(State *s);

	//this function returns the reward of the tuple <s,a,s_p> and whether the resultant state is a failure state or not
	double executeAction(State *s,Action *a,State *s_p);

	Reward *getRewardVector();
};


