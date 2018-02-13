#pragma once
#include <vector>
#include "../../3rd-party/tinyxml2/tinyxml2.h"

class CLink;
class CNetworkArchitecture;

class CChain
{
protected:
	vector<CLink*> m_chainLinks;
	string m_name;
	CChain(tinyxml2::XMLElement* pNode);

	CNetworkArchitecture* m_pParentNetworkArchitecture;
public:
	CChain();
	~CChain();

	const std::vector<CLink*> getChainLinks() const { return m_chainLinks; }
	void setChainLinks(vector<CLink*> pChainLinks) { m_chainLinks = pChainLinks; }
	const string& getName() const { return m_name; }

	const CLink* getLinkById(const char* id) const;

	void setParentNetworkArchitecture(CNetworkArchitecture* pParentNetworkArchitecture) { m_pParentNetworkArchitecture = pParentNetworkArchitecture; }
	const CNetworkArchitecture* getParentNetworkArchitecture() const { return m_pParentNetworkArchitecture; }

	static CChain* getInstance(tinyxml2::XMLElement* pNode);
};

