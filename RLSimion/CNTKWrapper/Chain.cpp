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
#include "CNTKWrapperInternals.h"

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
	m_name = CNTKWrapper::Internal::string2wstring((pParentNode->Attribute(XML_ATTRIBUTE_Name)));
	if (m_name.empty())
		m_name = L"Chain";

	//read links
	tinyxml2::XMLElement *pNode = pParentNode->FirstChildElement(XML_TAG_ChainLinks);
	if (pNode == nullptr)
		throw ProblemParserElementNotFound(XML_TAG_ChainLinks);

	loadChildren<Link>(pNode, XML_TAG_LinkBase, m_chainLinks);
	for (auto var : m_chainLinks)
	{
		var->setParentChain(this);
	}
}

const Link* Chain::getLinkById(const wstring id) const
{
	for (auto item : m_chainLinks)
	{
		if (item->getId()==id)
			return item;
	}

	return nullptr;
}
