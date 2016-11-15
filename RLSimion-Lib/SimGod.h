#pragma once

#include "parameters.h"
class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;
typedef CNamedVarSet CReward;
class CConfigNode;
class CSimion;
class CDeferredLoad;
class CFilePathList;
class CExperienceReplay;

#include <vector>

class CStateFeatureMap;
class CActionFeatureMap;

class CSimGod
{
	CHILD_OBJECT_FACTORY<CStateFeatureMap> m_pGlobalStateFeatureMap;
	CHILD_OBJECT_FACTORY<CActionFeatureMap> m_pGlobalActionFeatureMap;

	MULTI_VALUE_FACTORY<CSimion> m_simions;
		//std::vector<CSimion*> m_simions;

	CReward *m_pReward;

	//lists that must be initialized before the constructor is actually called
	std::vector<std::pair<CDeferredLoad*, unsigned int>> m_delayedLoadObjects;
	std::vector<const char*> m_inputFiles;
	std::vector<const char*> m_outputFiles;

	CHILD_OBJECT<CExperienceReplay> m_pExperienceReplay;
public:
	CSimGod();
	virtual ~CSimGod();

	void init(CConfigNode* pParameters);

	void selectAction(CState* s,CAction* a);
	void update(CState* s, CAction* a, CState* s_p, double r);

	//delayed load
	void registerDelayedLoadObj(CDeferredLoad* pObj,unsigned int orderLoad);
	void delayedLoad();

	//input/output files
	void registerInputFile(const char* filepath);
	void getInputFiles(CFilePathList& filepathList);

	void registerOutputFile(const char* filepath);
	void getOutputFiles(CFilePathList& filepathList);

	//global feature maps
	std::shared_ptr<CStateFeatureMap> getGlobalStateFeatureMap(){ return m_pGlobalStateFeatureMap.ptr(); }
	std::shared_ptr<CActionFeatureMap> getGlobalActionFeatureMap(){ return m_pGlobalActionFeatureMap.ptr(); }
};

