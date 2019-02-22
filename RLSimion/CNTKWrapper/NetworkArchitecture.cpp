/*
	SimionZoo: A framework for online model-free Reinforcement Learning on continuous
	control problems

	Copyright (c) 2016 SimionSoft. https://github.com/simionsoft

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#include "xmltags.h"
#include "NetworkArchitecture.h"
#include "Parameter.h"
#include "Chain.h"
#include "Link.h"
#include "Network.h"
#include "Parameter.h"
#include "ParameterValues.h"
#include "Exceptions.h"

NetworkArchitecture::NetworkArchitecture()
{
	m_chains = std::vector<Chain*>();
}


NetworkArchitecture::~NetworkArchitecture()
{

}

NetworkArchitecture::NetworkArchitecture(tinyxml2::XMLElement* pParentNode) : NetworkArchitecture()
{
	tinyxml2::XMLElement *pNode = pParentNode->FirstChildElement(XML_TAG_Chains);
	if (pNode == nullptr)
		throw ProblemParserElementNotFound(XML_TAG_Chains);

	loadChildren<Chain>(pNode, XML_TAG_Chain, m_chains);
	for (auto var : m_chains)
	{
		var->setParentNetworkArchitecture(this);
	}
}


NetworkArchitecture* NetworkArchitecture::getInstance(tinyxml2::XMLElement* pNode)
{
	if (!strcmp(pNode->Name(), XML_TAG_NETWORK_ARCHITECTURE))
		return new NetworkArchitecture(pNode);

	return nullptr;
}

const Link* NetworkArchitecture::getLinkById(const wstring& id) const
{
	const Link* result = NULL;
	for (auto cItem : m_chains)
	{
		result = cItem->getLinkById(id);
		if (result != NULL)
			break;
	}

	return result;
}