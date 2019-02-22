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

#include "InputData.h"
#include "xmltags.h"
#include "ParameterValues.h"
#include "CNTKWrapperInternals.h"
#include "Exceptions.h"



InputData::InputData(tinyxml2::XMLElement * pParentNode)
{
	m_id = pParentNode->Attribute(XML_ATTRIBUTE_Id);
	if (m_id.empty())
		throw ProblemParserElementNotFound(XML_ATTRIBUTE_Id);

	m_name = pParentNode->Attribute(XML_ATTRIBUTE_Name);
	if (m_name.empty())
		m_name = "InputData";

	tinyxml2::XMLElement* pNode = pParentNode->FirstChildElement(XML_TAG_Shape);
	if (pNode == nullptr)
		throw ProblemParserElementNotFound(XML_TAG_Shape);
	m_pShape = CIntTuple::getInstance(pNode);

}

InputData::InputData(string id, CNTK::Variable pInputVariable)
{
	m_id = id;
	m_inputVariable = pInputVariable;
}


InputData::~InputData()
{
}

InputData * InputData::getInstance(tinyxml2::XMLElement * pNode)
{
	if (!strcmp(pNode->Name(), XML_TAG_InputData))
		return new InputData(pNode);
	return nullptr;
}

NDShape InputData::getNDShape()
{
	return m_pShape->getNDShape();
}

void InputData::createInputVariable()
{
	m_isInitialized = true;
	m_inputVariable = CNTK::InputVariable(m_pShape->getNDShape(), CNTK::DataType::Double, CNTKWrapper::Internal::string2wstring(m_id));
}
