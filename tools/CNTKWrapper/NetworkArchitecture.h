#pragma once
#include <vector>
#include "../../3rd-party/tinyxml2/tinyxml2.h"

using namespace std;

class CChain;
class CLink;
class CProblem;

class CNetworkArchitecture
{
protected:
	vector<CChain*> m_chains;
	CProblem* m_pParentProblem;
	CNetworkArchitecture(tinyxml2::XMLElement* pNode);
	
public:
	CNetworkArchitecture();
	~CNetworkArchitecture();

	const vector<CChain*> getChains() { return m_chains; }

	void setParentProblem(CProblem* pParentProblem) { m_pParentProblem = pParentProblem; }
	const CProblem* getParentProblem() const { return m_pParentProblem; }

	static CNetworkArchitecture* getInstance(tinyxml2::XMLElement* pNode);

	const CLink* getLinkById(const char* id) const;
};

