#include "stdafx.h"
#include "InputData.h"
#include "xmltags.h"
#include "ParameterValues.h"
#include "CNTKWrapper.h"
#include "Exceptions.h"



CInputData::CInputData(tinyxml2::XMLElement * pParentNode)
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

CInputData::CInputData(string id, CNTK::Variable pInputVariable)
{
	m_id = id;
	m_pInputVariable = pInputVariable;
}


CInputData::~CInputData()
{
}

CInputData * CInputData::getInstance(tinyxml2::XMLElement * pNode)
{
	if (!strcmp(pNode->Name(), XML_TAG_InputData))
		return new CInputData(pNode);
	return nullptr;
}

NDShape CInputData::getNDShape()
{
	return m_pShape->getNDShape();
}

void CInputData::createInputVariable()
{
	m_pInputVariable = CNTK::InputVariable(m_pShape->getNDShape(), CNTK::DataType::Float, CNTKWrapper::Internal::string2wstring(m_id));
}
