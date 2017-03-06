#pragma once

#include "parameters.h"
#include <vector>
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

class CSimGod
{
	static CHILD_OBJECT_FACTORY<CStateFeatureMap> m_pGlobalStateFeatureMap;
	static CHILD_OBJECT_FACTORY<CActionFeatureMap> m_pGlobalActionFeatureMap;

	MULTI_VALUE_FACTORY<CSimion> m_simions;
	
	DOUBLE_PARAM m_gamma;

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

	void selectAction(CState* s,CAction* a);
	void update(CState* s, CAction* a, CState* s_p, double r);

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
};

