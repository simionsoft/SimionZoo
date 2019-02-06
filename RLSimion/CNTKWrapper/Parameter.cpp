#include "xmltags.h"
#include "NetworkArchitecture.h"
#include "Parameter.h"
#include "Chain.h"
#include "Link.h"
#include "Network.h"
#include "Parameter.h"
#include "Exceptions.h"
#include "ParameterValues.h"

template <typename T>
ParameterBase<T>::~ParameterBase()
{

}

template <typename T>
ParameterBase<T>::ParameterBase(tinyxml2::XMLElement* pNode)
{
	m_name = pNode->Attribute(XML_ATTRIBUTE_Name);
}

DoubleParameter::DoubleParameter(tinyxml2::XMLElement* pParentNode) : ParameterBase<double>(pParentNode)
{
	tinyxml2::XMLElement *pNode = pParentNode->FirstChildElement(XML_TAG_Value);

	const char* rawValue = pNode->GetText();

	m_value = atof(rawValue);
}

ActivationFunctionParameter::ActivationFunctionParameter(tinyxml2::XMLElement* pParentNode) : ParameterBase<ActivationFunction>(pParentNode)
{
	tinyxml2::XMLElement *pNode = pParentNode->FirstChildElement(XML_TAG_Value);
	const char* type = pNode->GetText();
	if (!strcmp(type, "elu"))
		m_value = ActivationFunction::elu;
	else if (!strcmp(type, "selu"))
		m_value = ActivationFunction::selu;
	else if (!strcmp(type, "softplus"))
		m_value = ActivationFunction::softplus;
	else if (!strcmp(type, "softsign"))
		m_value = ActivationFunction::softsign;
	else if (!strcmp(type, "relu"))
		m_value = ActivationFunction::relu;
	else if (!strcmp(type, "tanh"))
		m_value = ActivationFunction::tanh;
	else if (!strcmp(type, "sigmoid"))
		m_value = ActivationFunction::sigmoid;
	else if (!strcmp(type, "hard_sigmoid"))
		m_value = ActivationFunction::hard_sigmoid;
	else if (!strcmp(type, "softmax"))
		m_value = ActivationFunction::softmax;
	else if (!strcmp(type, "linear"))
		m_value = ActivationFunction::linear;
	else
		throw ProblemParserElementValueNotValid(XML_TAG_Value);
}

InputDataParameter::InputDataParameter(tinyxml2::XMLElement* pParentNode) : ParameterBase<string>(pParentNode)
{
	tinyxml2::XMLElement *pNode = pParentNode->FirstChildElement(XML_TAG_Value);
	m_value = pNode->GetText();
}

IntParameter::IntParameter(tinyxml2::XMLElement* pParentNode) : ParameterBase<int>(pParentNode)
{
	tinyxml2::XMLElement *pNode = pParentNode->FirstChildElement(XML_TAG_Value);
	const char* rawValue = pNode->GetText();
	m_value = stoi(rawValue);
}

IntTuple1DParameter::IntTuple1DParameter(tinyxml2::XMLElement* pParentNode) : ParameterBase<CIntTuple1D>(pParentNode)
{
	tinyxml2::XMLElement *pNode = pParentNode->FirstChildElement(XML_TAG_Value);
	m_value = *CIntTuple1D::getInstance(pNode);
}

IntTuple2DParameter::IntTuple2DParameter(tinyxml2::XMLElement* pParentNode) : ParameterBase<CIntTuple2D>(pParentNode)
{
	tinyxml2::XMLElement *pNode = pParentNode->FirstChildElement(XML_TAG_Value);
	m_value = *CIntTuple2D::getInstance(pNode);
}

IntTuple3DParameter::IntTuple3DParameter(tinyxml2::XMLElement* pParentNode) : ParameterBase<CIntTuple3D>(pParentNode)
{
	tinyxml2::XMLElement *pNode = pParentNode->FirstChildElement(XML_TAG_Value);
	m_value = *CIntTuple3D::getInstance(pNode);
}

IntTuple4DParameter::IntTuple4DParameter(tinyxml2::XMLElement* pParentNode) : ParameterBase<CIntTuple4D>(pParentNode)
{
	tinyxml2::XMLElement *pNode = pParentNode->FirstChildElement(XML_TAG_Value);
	m_value = *CIntTuple4D::getInstance(pNode);
}

IParameter* IParameter::getInstance(tinyxml2::XMLElement* pNode)
{
	if (!strcmp(pNode->Name(), XML_TAG_ParameterBase))
	{
		const char* type = pNode->Attribute(XML_ATTRIBUTE_Type);
		if (!strcmp(type, XML_ATTRIBUTE_ActivationFunctionParameter))
			return new ActivationFunctionParameter(pNode);
		else if (!strcmp(type, XML_ATTRIBUTE_DoubleParameter))
			return new DoubleParameter(pNode);
		else if (!strcmp(type, XML_ATTRIBUTE_InputDataParameter))
			return new InputDataParameter(pNode);
		else if (!strcmp(type, XML_ATTRIBUTE_IntParameter))
			return new IntParameter(pNode);
		else if (!strcmp(type, XML_ATTRIBUTE_IntTuple1DParameter))
			return new IntTuple1DParameter(pNode);
		else if (!strcmp(type, XML_ATTRIBUTE_IntTuple2DParameter))
			return new IntTuple2DParameter(pNode);
		else if (!strcmp(type, XML_ATTRIBUTE_IntTuple3DParameter))
			return new IntTuple3DParameter(pNode);
		else if (!strcmp(type, XML_ATTRIBUTE_IntTuple4DParameter))
			return new IntTuple4DParameter(pNode);
		else if (!strcmp(type, XML_ATTRIBUTE_LinkConnectionListParameter))
			return new LinkConnectionListParameter(pNode);
		else
			throw ProblemParserElementValueNotValid(XML_ATTRIBUTE_Type);
	}

	return nullptr;
}

LinkConnectionListParameter::LinkConnectionListParameter(tinyxml2::XMLElement * pParentNode) : ParameterBase<vector<LinkConnection*>>(pParentNode)
{
	tinyxml2::XMLElement *pNode = pParentNode->FirstChildElement(XML_TAG_Value);
	loadChildren<LinkConnection>(pNode, XML_TAG_LinkConnection, m_value);
}
