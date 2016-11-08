#pragma once


class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;
typedef CNamedVarSet CReward;
class CParameters;
class CSimion;
class CDeferredLoad;
class CFilePathList;
class CExperienceReplay;

#include <vector>

class CStateFeatureMap;
class CActionFeatureMap;

class CSimGod
{
	CStateFeatureMap* m_pGlobalStateFeatureMap;
	CActionFeatureMap* m_pGlobalActionFeatureMap;

	int m_numSimions= 0;
	CSimion** m_pSimions = 0;

	CReward *m_pReward;

	//lists that must be initialized before the constructor is actually called
	std::vector<std::pair<CDeferredLoad*, unsigned int>> m_delayedLoadObjects;
	std::vector<const char*> m_inputFiles;
	std::vector<const char*> m_outputFiles;

	CExperienceReplay* m_pExperienceReplay;
public:
	CSimGod();
	virtual ~CSimGod();

	void init(CParameters* pParameters);

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
	CStateFeatureMap* getGlobalStateFeatureMap(){ return m_pGlobalStateFeatureMap; }
	CActionFeatureMap* getGlobalActionFeatureMap(){ return m_pGlobalActionFeatureMap; }
};

