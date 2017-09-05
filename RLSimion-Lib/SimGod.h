#pragma once

#include "parameters.h"
#include <vector>
#include "mem-manager.h"
class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;
typedef CNamedVarSet CReward;
class CConfigNode;
class CSimion;
class CFilePathList;
class CExperienceReplay;
class CDeferredLoad;
class CStateFeatureMap;
class CActionFeatureMap;
class CFeatureList;


//This class is the Simion God: it controls the learning agents and holds global learning parameters
//Some members are declared static because they are requested by children before the SimGod object is actually constructed
class CSimGod
{
	static CHILD_OBJECT_FACTORY<CStateFeatureMap> m_pGlobalStateFeatureMap;
	static CHILD_OBJECT_FACTORY<CActionFeatureMap> m_pGlobalActionFeatureMap;

	bool m_bReplayingExperience= false;

	MULTI_VALUE_FACTORY<CSimion> m_simions;
	
	DOUBLE_PARAM m_gamma;

	BOOL_PARAM m_bFreezeTargetFunctions;
	INT_PARAM m_targetFunctionUpdateFreq;
	BOOL_PARAM m_bUseImportanceWeights;

	CReward *m_pReward;

	//lists that must be initialized before the constructor is actually called
	static std::vector<std::pair<CDeferredLoad*, unsigned int>> m_deferredLoadSteps;
	static std::vector<const char*> m_inputFiles;
	static std::vector<const char*> m_outputFiles;

	CHILD_OBJECT<CExperienceReplay> m_pExperienceReplay;
public:
	CSimGod(CConfigNode* pParameters);
	CSimGod() = default;
	virtual ~CSimGod();

	bool bReplayingExperience() const { return m_bReplayingExperience; }

	double selectAction(CState* s,CAction* a);
	//regular update step after a simulation time-step
	//variables will be logged after this step
	void update(CState* s, CAction* a, CState* s_p, double r, double probability);
	//post-update step done after logging variables
	//used to avoid having experience replay mess with the stats logged
	void postUpdate();

	//delayed load
	static void registerDeferredLoadStep(CDeferredLoad* deferredLoadObject,unsigned int orderLoad);
	void deferredLoad();

	//input/output files
	static void registerInputFile(const char* filepath);
	void getInputFiles(CFilePathList& filepathList);

	static void registerOutputFile(const char* filepath);
	void getOutputFiles(CFilePathList& filepathList);

	//global feature maps
	static std::shared_ptr<CStateFeatureMap> getGlobalStateFeatureMap();
	static std::shared_ptr<CActionFeatureMap> getGlobalActionFeatureMap();

	//global learning parameters
	double getGamma();

	//Target-Function freeze
	//returns the freq. (in steps) at which the V-functions should be updated (0 if they aren't deferred)
	int getTargetFunctionUpdateFreq();

	//Use sample importance weights for off-policy learning?
	bool useSampleImportanceWeights();
};

