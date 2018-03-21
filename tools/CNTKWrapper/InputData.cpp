#include "stdafx.h"

#ifdef _WIN64




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
	m_inputVariable = CNTK::InputVariable(m_pShape->getNDShape(), CNTK::DataType::Double, CNTKWrapper::Internal::string2wstring(m_id));
}

#endif // _WIN64