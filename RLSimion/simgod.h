#pragma once

#include "parameters.h"
#include <vector>
#include "mem-manager.h"
class NamedVarSet;
typedef NamedVarSet State;
typedef NamedVarSet Action;
typedef NamedVarSet Reward;
class ConfigNode;
class Simion;
class ExperienceReplay;
class DeferredLoad;
class StateFeatureMap;
class ActionFeatureMap;
class FeatureList;


//This class is the Simion God: it controls the learning agents and holds global learning parameters
//Some members are declared static because they are requested by children before the SimGod object is actually constructed
class SimGod
{
	static CHILD_OBJECT_FACTORY<StateFeatureMap> m_pGlobalStateFeatureMap;
	static CHILD_OBJECT_FACTORY<ActionFeatureMap> m_pGlobalActionFeatureMap;

	bool m_bReplayingExperience= false;

	MULTI_VALUE_FACTORY<Simion> m_simions;
	
	DOUBLE_PARAM m_gamma;

	BOOL_PARAM m_bFreezeTargetFunctions;
	INT_PARAM m_targetFunctionUpdateFreq;
	BOOL_PARAM m_bUseImportanceWeights;

	Reward *m_pReward;

	//lists that must be initialized before the constructor is actually called
	static std::vector<std::pair<DeferredLoad*, unsigned int>> m_deferredLoadSteps;

	CHILD_OBJECT<ExperienceReplay> m_pExperienceReplay;
public:
	SimGod(ConfigNode* pParameters);
	SimGod() = default;
	virtual ~SimGod();

	bool bReplayingExperience() const { return m_bReplayingExperience; }

	double selectAction(State* s,Action* a);
	//regular update step after a simulation time-step
	//variables will be logged after this step
	void update(State* s, Action* a, State* s_p, double r, double probability);
	//post-update step done after logging variables
	//used to avoid having experience replay mess with the stats logged
	void postUpdate();

	//delayed load
	static void registerDeferredLoadStep(DeferredLoad* deferredLoadObject,unsigned int orderLoad);
	void deferredLoad();

	//global feature maps
	static std::shared_ptr<StateFeatureMap> getGlobalStateFeatureMap();
	static std::shared_ptr<ActionFeatureMap> getGlobalActionFeatureMap();

	//global learning parameters
	double getGamma();

	//Target-Function freeze
	//returns the freq. (in steps) at which the V-functions should be updated (0 if they aren't deferred)
	int getTargetFunctionUpdateFreq();

	//Use sample importance weights for off-policy learning?
	bool useSampleImportanceWeights();
};

