#pragma once
#include <vector>
#include "../../3rd-party/tinyxml2/tinyxml2.h"

using namespace std;

class Chain;
class Link;
class NetworkDefinition;

class NetworkArchitecture
{
protected:
	vector<Chain*> m_chains;
	NetworkDefinition* m_pNetworkDefinition;
	NetworkArchitecture(tinyxml2::XMLElement* pNode);
	
public:
	NetworkArchitecture();
	~NetworkArchitecture();

	const vector<Chain*>& getChains() { return m_chains; }

	void setParentProblem(NetworkDefinition* pParentProblem) { m_pNetworkDefinition = pParentProblem; }
	NetworkDefinition* getParentProblem() { return m_pNetworkDefinition; }

	static NetworkArchitecture* getInstance(tinyxml2::XMLElement* pNode);

	const Link* getLinkById(const char* id) const;
};

