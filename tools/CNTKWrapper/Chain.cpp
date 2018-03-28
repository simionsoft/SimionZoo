#ifdef _WIN64

#include "xmltags.h"
#include "NetworkArchitecture.h"
#include "Parameter.h"
#include "Chain.h"
#include "Link.h"
#include "Network.h"
#include "Parameter.h"
#include "ParameterValues.h"
#include "Exceptions.h"

Chain::Chain()
{
	m_chainLinks = std::vector<Link*>();
}


Chain::~Chain()
{
}

Chain::Chain(tinyxml2::XMLElement* pParentNode) : Chain()
{
	//read name
	m_name = pParentNode->Attribute(XML_ATTRIBUTE_Name);
	if (m_name.empty())
		m_name = "Chain";

	//read links
	tinyxml2::XMLElement *pNode = pParentNode->FirstChildElement(XML_TAG_ChainLinks);
	if (pNode == nullptr)
		throw ProblemParserElementNotFound(XML_TAG_ChainLinks);

	loadChildren<Link>(pNode, XML_TAG_LinkBase, m_chainLinks);
	for each (auto var in m_chainLinks)
	{
		var->setParentChain(this);
	}
}

const Link* Chain::getLinkById(const char * id) const
{
	for each (auto item in m_chainLinks)
	{
		if (!strcmp(item->getId().c_str(), id))
		{
			return item;
		}
	}

	return NULL;
}

#endif