#include "stdafx.h"
#include "xmltags.h"
#include "NetworkArchitecture.h"
#include "Parameter.h"
#include "Chain.h"
#include "Link.h"
#include "Network.h"
#include "Parameter.h"
#include "ParameterValues.h"
#include "Exceptions.h"

CNetworkArchitecture::CNetworkArchitecture()
{
	m_chains = std::vector<CChain*>();
}


CNetworkArchitecture::~CNetworkArchitecture()
{

}

CNetworkArchitecture::CNetworkArchitecture(tinyxml2::XMLElement* pParentNode) : CNetworkArchitecture()
{
	tinyxml2::XMLElement *pNode = pParentNode->FirstChildElement(XML_TAG_Chains);
	if (pNode == nullptr)
		throw ProblemParserElementNotFound(XML_TAG_Chains);

	loadChildren<CChain>(pNode, XML_TAG_Chain, m_chains);
	for each (auto var in m_chains)
	{
		var->setParentNetworkArchitecture(this);
	}
}


CNetworkArchitecture* CNetworkArchitecture::getInstance(tinyxml2::XMLElement* pNode)
{
	if (!strcmp(pNode->Name(), XML_TAG_NETWORK_ARCHITECTURE))
		return new CNetworkArchitecture(pNode);

	return nullptr;
}

const CLink* CNetworkArchitecture::getLinkById(const char* id) const
{
	const CLink* result = NULL;
	for each (auto cItem in m_chains)
	{
		result = cItem->getLinkById(id);
		if (result != NULL)
			break;
	}

	return result;
}