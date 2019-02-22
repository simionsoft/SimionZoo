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
#include "NetworkDefinition.h"
#include "Exceptions.h"
#include "CNTKWrapperInternals.h"

Link::Link()
{
}


Link::~Link()
{
}

Link::Link(tinyxml2::XMLElement* pParentNode)
{
	//read type
	const char* type = pParentNode->Attribute(XML_ATTRIBUTE_Type);
	if (!strcmp(type, "InputLayer"))
		m_linkType = LinkType::InputLayer;
	else if (!strcmp(type, "ActivationLayer"))
		m_linkType = LinkType::ActivationLayer;
	else if (!strcmp(type, "Convolution1DLayer"))
		m_linkType = LinkType::Convolution1DLayer;
	else if (!strcmp(type, "Convolution2DLayer"))
		m_linkType = LinkType::Convolution2DLayer;
	else if (!strcmp(type, "Convolution3DLayer"))
		m_linkType = LinkType::Convolution3DLayer;
	else if (!strcmp(type, "DenseLayer"))
		m_linkType = LinkType::DenseLayer;
	else if (!strcmp(type, "DropoutLayer"))
		m_linkType = LinkType::DropoutLayer;
	else if (!strcmp(type, "FlattenLayer"))
		m_linkType = LinkType::FlattenLayer;
	else if (!strcmp(type, "ReshapeLayer"))
		m_linkType = LinkType::ReshapeLayer;
	else if (!strcmp(type, "MergeLayer"))
		m_linkType = LinkType::MergeLayer;
	else if (!strcmp(type, "BatchNormalizationLayer"))
		m_linkType = LinkType::BatchNormalizationLayer;
	else if (!strcmp(type, "LinearTransformationLayer"))
		m_linkType = LinkType::LinearTransformationLayer;
	else
		throw ProblemParserElementValueNotValid(XML_ATTRIBUTE_Type);

	//read name
	m_name = CNTKWrapper::Internal::string2wstring(pParentNode->Attribute(XML_ATTRIBUTE_Name));
	if (m_name.empty())
		m_name = L"Link";

	//read id
	m_id = CNTKWrapper::Internal::string2wstring(pParentNode->Attribute(XML_ATTRIBUTE_Id));
	if (m_id.empty())
		throw ProblemParserElementValueNotValid(XML_ATTRIBUTE_Id);

	//read parameters
	tinyxml2::XMLElement *pNode = pParentNode->FirstChildElement(XML_TAG_Parameters);
	if (pNode == nullptr)
		throw ProblemParserElementNotFound(XML_TAG_Parameters);

	loadChildren<IParameter>(pNode, XML_TAG_ParameterBase, m_parameters);
}

std::vector<const Link*> Link::getDependencies() const
{
	std::vector<const Link*> result = std::vector<const Link*>();

	if (m_linkType == LinkType::MergeLayer)
	{
		auto dependenciesList = getParameterByName<LinkConnectionListParameter>("Links");
		for (LinkConnection* var : dependenciesList->getValue())
			result.push_back(m_pParentChain->getNetworkArchitecture()->getLinkById(var->getTargetId()));
	}
	else
	{
		//just add the item which is placed in front of this item in the chain
		Link* previousLink = getPreviousLink();
		if (previousLink != nullptr)
			result.push_back(previousLink);
	}

	return result;
}

void Link::createCNTKFunctionPtr()
{
	createCNTKFunctionPtr(getDependencies());
}

void Link::createCNTKFunctionPtr(vector<const Link*> dependencies)
{
	auto device = CNTK::DeviceDescriptor::UseDefaultDevice();
	switch (m_linkType)
	{
	case LinkType::InputLayer:
		m_functionPtr = CNTKWrapper::InputLayer(this, dependencies, device);
		break;
	case LinkType::ActivationLayer:
		m_functionPtr = CNTKWrapper::ActivationLayer(this, dependencies, device);
		break;
	case LinkType::Convolution1DLayer:
		m_functionPtr = CNTKWrapper::Convolution1DLayer(this, dependencies, device);
		break;
	case LinkType::Convolution2DLayer:
		m_functionPtr = CNTKWrapper::Convolution2DLayer(this, dependencies, device);
		break;
	case LinkType::Convolution3DLayer:
		m_functionPtr = CNTKWrapper::Convolution3DLayer(this, dependencies, device);
		break;
	case LinkType::DenseLayer:
		m_functionPtr = CNTKWrapper::DenseLayer(this, dependencies, device);
		break;
	case LinkType::DropoutLayer:
		m_functionPtr = CNTKWrapper::DropoutLayer(this, dependencies, device);
		break;
	case LinkType::FlattenLayer:
		m_functionPtr = CNTKWrapper::FlattenLayer(this, dependencies, device);
		break;
	case LinkType::ReshapeLayer:
		m_functionPtr = CNTKWrapper::ReshapeLayer(this, dependencies, device);
		break;
	case LinkType::MergeLayer:
		m_functionPtr = CNTKWrapper::MergeLayer(this, dependencies, device);
		break;
	case LinkType::BatchNormalizationLayer:
		m_functionPtr = CNTKWrapper::BatchNormalizationLayer(this, dependencies, device);
		break;
	case LinkType::LinearTransformationLayer:
		m_functionPtr = CNTKWrapper::LinearTransformationLayer(this, dependencies, device);
		break;
	}
}


Link * Link::getNextLink() const
{
	//determine position and get the next element
	vector<Link*> chainLinks = m_pParentChain->getLinks();
	size_t position = std::distance(chainLinks.begin(), std::find(chainLinks.begin(), chainLinks.end(), this));
	if (position >= chainLinks.size() - 1)
		return nullptr;
	return chainLinks.at(position + 1);
}

Link * Link::getPreviousLink() const
{
	//determine position and get the previous element
	vector<Link*> chainLinks = m_pParentChain->getLinks();
	ptrdiff_t position = std::distance(chainLinks.begin(), std::find(chainLinks.begin(), chainLinks.end(), this));
	if (position <= 0)
		return nullptr;
	return chainLinks.at(position - 1);
}

Chain* Chain::getInstance(tinyxml2::XMLElement* pNode)
{
	if (!strcmp(pNode->Name(), XML_TAG_Chain))
		return new Chain(pNode);

	return nullptr;
}

Link* Link::getInstance(tinyxml2::XMLElement* pNode)
{
	if (!strcmp(pNode->Name(), XML_TAG_LinkBase))
		return new Link(pNode);

	return nullptr;
}