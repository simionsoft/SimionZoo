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

#include "CNTKLibrary.h"
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

CIntTuple1D::CIntTuple1D(tinyxml2::XMLElement* pParentNode) : CIntTuple1D()
{
	tinyxml2::XMLElement *pNode = pParentNode->FirstChildElement(XML_TAG_Tuple_1);
	if (pNode == nullptr)
		throw ProblemParserElementNotFound(XML_TAG_Tuple_1);
	m_x1 = stoi(pNode->GetText());
}

CNTK::NDShape CIntTuple1D::getNDShape()
{
	return { (size_t)m_x1 };
}


CIntTuple2D::CIntTuple2D(tinyxml2::XMLElement* pParentNode) : CIntTuple2D()
{
	tinyxml2::XMLElement *pNode = pParentNode->FirstChildElement(XML_TAG_Tuple_1);
	if (pNode == nullptr)
		throw ProblemParserElementNotFound(XML_TAG_Tuple_1);
	m_x1 = stoi(pNode->GetText());

	pNode = pParentNode->FirstChildElement(XML_TAG_Tuple_2);
	if (pNode == nullptr)
		throw ProblemParserElementNotFound(XML_TAG_Tuple_2);
	m_x2 = stoi(pNode->GetText());
}

CNTK::NDShape CIntTuple2D::getNDShape()
{
	return { (size_t)m_x1, (size_t)m_x2 };
}

CIntTuple3D::CIntTuple3D(tinyxml2::XMLElement* pParentNode) : CIntTuple3D()
{
	tinyxml2::XMLElement *pNode = pParentNode->FirstChildElement(XML_TAG_Tuple_1);
	if (pNode == nullptr)
		throw ProblemParserElementNotFound(XML_TAG_Tuple_1);
	m_x1 = stoi(pNode->GetText());

	pNode = pParentNode->FirstChildElement(XML_TAG_Tuple_2);
	if (pNode == nullptr)
		throw ProblemParserElementNotFound(XML_TAG_Tuple_1);
	m_x2 = stoi(pNode->GetText());

	pNode = pParentNode->FirstChildElement(XML_TAG_Tuple_3);
	if (pNode == nullptr)
		throw ProblemParserElementNotFound(XML_TAG_Tuple_3);
	m_x3 = stoi(pNode->GetText());
}

CNTK::NDShape CIntTuple3D::getNDShape()
{
	return { (size_t)m_x1, (size_t)m_x2, (size_t)m_x3 };
}

CIntTuple4D::CIntTuple4D(tinyxml2::XMLElement* pParentNode) : CIntTuple4D()
{
	tinyxml2::XMLElement *pNode = pParentNode->FirstChildElement(XML_TAG_Tuple_1);
	if (pNode == nullptr)
		throw ProblemParserElementNotFound(XML_TAG_Tuple_1);
	m_x1 = stoi(pNode->GetText());

	pNode = pParentNode->FirstChildElement(XML_TAG_Tuple_2);
	if (pNode == nullptr)
		throw ProblemParserElementNotFound(XML_TAG_Tuple_2);
	m_x2 = stoi(pNode->GetText());

	pNode = pParentNode->FirstChildElement(XML_TAG_Tuple_3);
	if (pNode == nullptr)
		throw ProblemParserElementNotFound(XML_TAG_Tuple_3);
	m_x3 = stoi(pNode->GetText());

	pNode = pParentNode->FirstChildElement(XML_TAG_Tuple_4);
	if (pNode == nullptr)
		throw ProblemParserElementNotFound(XML_TAG_Tuple_4);
	m_x4 = stoi(pNode->GetText());
}

CNTK::NDShape CIntTuple4D::getNDShape()
{
	return { (size_t)m_x1, (size_t)m_x2, (size_t)m_x3 };
}

CIntTuple1D* CIntTuple1D::getInstance(tinyxml2::XMLElement* pNode)
{
	//if (!strcmp(pNode->Attribute(XML_ATTRIBUTE_Type), XML_TAG_IntTuple1D))
	return new CIntTuple1D(pNode);
}


CIntTuple2D* CIntTuple2D::getInstance(tinyxml2::XMLElement* pNode)
{
	//if (!strcmp(pNode->Attribute(XML_ATTRIBUTE_Type), XML_TAG_IntTuple2D))
	return new CIntTuple2D(pNode);
}

CIntTuple3D* CIntTuple3D::getInstance(tinyxml2::XMLElement* pNode)
{
	//if (!strcmp(pNode->Attribute(XML_ATTRIBUTE_Type), XML_TAG_IntTuple3D))
	return new CIntTuple3D(pNode);
}

CIntTuple4D* CIntTuple4D::getInstance(tinyxml2::XMLElement* pNode)
{
	//if (!strcmp(pNode->Attribute(XML_ATTRIBUTE_Type), XML_TAG_IntTuple4D))
	return new CIntTuple4D(pNode);
}

CInputDataValue* CInputDataValue::getInstance(tinyxml2::XMLElement* pNode)
{
	//some type checking would be good, but due to C#, this is not possible atm.
	return new CInputDataValue(pNode);
}

LinkConnection* LinkConnection::getInstance(tinyxml2::XMLElement* pNode)
{
	if (!strcmp(pNode->Name(), XML_TAG_LinkConnection))
	{
		return new LinkConnection(pNode);
	}
	return nullptr;
}

CInputDataValue::CInputDataValue(tinyxml2::XMLElement* pParentNode) : CInputDataValue()
{
	m_id = pParentNode->Attribute(XML_ATTRIBUTE_Id);
	if (m_id.empty())
		throw ProblemParserElementValueNotValid(XML_ATTRIBUTE_Id);

	m_name = pParentNode->Attribute(XML_ATTRIBUTE_Name);
	if (m_name.empty())
		m_name = "Input";

	tinyxml2::XMLElement* pNode = pParentNode->FirstChildElement(XML_TAG_Shape);
	if (pNode == nullptr)
		throw ProblemParserElementNotFound(XML_TAG_Shape);
	m_pShape = CIntTuple::getInstance(pNode);
}

LinkConnection::LinkConnection(tinyxml2::XMLElement* pNode) : LinkConnection()
{
	m_targetID = CNTKWrapper::Internal::string2wstring(pNode->Attribute(XML_ATTRIBUTE_TargetId));
	if (m_targetID.empty())
		throw ProblemParserElementValueNotValid(XML_ATTRIBUTE_TargetId);
}

LinkConnection::LinkConnection()
{
}

LinkConnection::~LinkConnection()
{
}

CIntTuple * CIntTuple::getInstance(tinyxml2::XMLElement * pNode)
{
	string type = pNode->Attribute(XML_ATTRIBUTE_Type);
	if (!strcmp(type.c_str(), XML_TAG_IntTuple1D))
		return CIntTuple1D::getInstance(pNode);
	else if (!strcmp(type.c_str(), XML_TAG_IntTuple2D))
		return CIntTuple2D::getInstance(pNode);
	else if (!strcmp(type.c_str(), XML_TAG_IntTuple3D))
		return CIntTuple3D::getInstance(pNode);
	else if (!strcmp(type.c_str(), XML_TAG_IntTuple4D))
		return CIntTuple4D::getInstance(pNode);
	return nullptr;
}
