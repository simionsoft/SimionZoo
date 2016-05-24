#pragma once


class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;
typedef CNamedVarSet CReward;
class CParameters;
class CSimion;
class CDeferredLoad;
class CFilePathList;

#include <vector>


class CSimGod
{
	int m_numSimions= 0;
	CSimion** m_pSimions = 0;

	CReward *m_pReward;
	std::vector<std::pair<CDeferredLoad*, unsigned int>> m_delayedLoadObjects;

	std::vector<const char*> m_inputFiles;
	std::vector<const char*> m_outputFiles;
public:
	CSimGod();
	virtual ~CSimGod();

	void init(CParameters* pParameters);

	void selectAction(CState* s,CAction* a);
	void update(CState* s, CAction* a, CState* s_p, double r);

	void registerDelayedLoadObj(CDeferredLoad* pObj,unsigned int orderLoad);
	void delayedLoad();

	void registerInputFile(const char* filepath);
	void getInputFiles(CFilePathList& filepathList);

	void registerOutputFile(const char* filepath);
	void getOutputFiles(CFilePathList& filepathList);
};

