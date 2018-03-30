#pragma once
#include <vector>
#include <string>
using namespace std;
#include "../../3rd-party/tinyxml2/tinyxml2.h"

class Link;
class NetworkArchitecture;

class Chain
{
protected:
	vector<Link*> m_chainLinks;
	string m_name;
	Chain(tinyxml2::XMLElement* pNode);

	NetworkArchitecture* m_pParentNetworkArchitecture;
public:
	Chain();
	~Chain();

	const std::vector<Link*> getChainLinks() const { return m_chainLinks; }
	void setChainLinks(vector<Link*> pChainLinks) { m_chainLinks = pChainLinks; }
	const string& getName() const { return m_name; }

	const Link* getLinkById(const char* id) const;

	void setParentNetworkArchitecture(NetworkArchitecture* pParentNetworkArchitecture) { m_pParentNetworkArchitecture = pParentNetworkArchitecture; }
	NetworkArchitecture* getParentNetworkArchitecture() const { return m_pParentNetworkArchitecture; }

	static Chain* getInstance(tinyxml2::XMLElement* pNode);
};

