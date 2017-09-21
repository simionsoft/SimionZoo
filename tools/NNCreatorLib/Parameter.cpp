#include "stdafx.h"
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
CParameterBase<T>::~CParameterBase()
{

}

template <typename T>
CParameterBase<T>::CParameterBase(tinyxml2::XMLElement* pNode)
{
	m_name = pNode->Attribute(XML_ATTRIBUTE_Name);
}

CDoubleParameter::CDoubleParameter(tinyxml2::XMLElement* pParentNode) : CParameterBase<double>(pParentNode)
{

}

CActivationFunctionParameter::CActivationFunctionParameter(tinyxml2::XMLElement* pParentNode) : CParameterBase<ActivationFunction>(pParentNode)
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

CInputDataParameter::CInputDataParameter(tinyxml2::XMLElement* pParentNode) : CParameterBase<string>(pParentNode)
{
	tinyxml2::XMLElement *pNode = pParentNode->FirstChildElement(XML_TAG_Value);
	m_value = pNode->GetText();
}

CIntParameter::CIntParameter(tinyxml2::XMLElement* pParentNode) : CParameterBase<int>(pParentNode)
{
	tinyxml2::XMLElement *pNode = pParentNode->FirstChildElement(XML_TAG_Value);
	const char* rawValue = pNode->GetText();
	m_value = stoi(rawValue);
}

CIntTuple1DParameter::CIntTuple1DParameter(tinyxml2::XMLElement* pParentNode) : CParameterBase<CIntTuple1D>(pParentNode)
{
	tinyxml2::XMLElement *pNode = pParentNode->FirstChildElement(XML_TAG_Value);
	m_value = *CIntTuple1D::getInstance(pNode);
}

CIntTuple2DParameter::CIntTuple2DParameter(tinyxml2::XMLElement* pParentNode) : CParameterBase<CIntTuple2D>(pParentNode)
{
	tinyxml2::XMLElement *pNode = pParentNode->FirstChildElement(XML_TAG_Value);
	m_value = *CIntTuple2D::getInstance(pNode);
}

CIntTuple3DParameter::CIntTuple3DParameter(tinyxml2::XMLElement* pParentNode) : CParameterBase<CIntTuple3D>(pParentNode)
{
	tinyxml2::XMLElement *pNode = pParentNode->FirstChildElement(XML_TAG_Value);
	m_value = *CIntTuple3D::getInstance(pNode);
}

CIntTuple4DParameter::CIntTuple4DParameter(tinyxml2::XMLElement* pParentNode) : CParameterBase<CIntTuple4D>(pParentNode)
{
	tinyxml2::XMLElement *pNode = pParentNode->FirstChildElement(XML_TAG_Value);
	m_value = *CIntTuple4D::getInstance(pNode);
}

CParameterBaseInteface* CParameterBaseInteface::getInstance(tinyxml2::XMLElement* pNode)
{
	if (!strcmp(pNode->Name(), XML_TAG_ParameterBase))
	{
		const char* type = pNode->Attribute(XML_ATTRIBUTE_Type);
		if (!strcmp(type, XML_ATTRIBUTE_ActivationFunctionParameter))
			return new CActivationFunctionParameter(pNode);
		else if (!strcmp(type, XML_ATTRIBUTE_DoubleParameter))
			return new CDoubleParameter(pNode);
		else if (!strcmp(type, XML_ATTRIBUTE_InputDataParameter))
			return new CInputDataParameter(pNode);
		else if (!strcmp(type, XML_ATTRIBUTE_IntParameter))
			return new CIntParameter(pNode);
		else if (!strcmp(type, XML_ATTRIBUTE_IntTuple1DParameter))
			return new CIntTuple1DParameter(pNode);
		else if (!strcmp(type, XML_ATTRIBUTE_IntTuple2DParameter))
			return new CIntTuple2DParameter(pNode);
		else if (!strcmp(type, XML_ATTRIBUTE_IntTuple3DParameter))
			return new CIntTuple3DParameter(pNode);
		else if (!strcmp(type, XML_ATTRIBUTE_IntTuple4DParameter))
			return new CIntTuple4DParameter(pNode);
		else if (!strcmp(type, XML_ATTRIBUTE_LinkConnectionListParameter))
			return new CLinkConnectionListParameter(pNode);
		else
			throw ProblemParserElementValueNotValid(XML_ATTRIBUTE_Type);
	}

	return nullptr;
}

CLinkConnectionListParameter::CLinkConnectionListParameter(tinyxml2::XMLElement * pParentNode) : CParameterBase<vector<CLinkConnection*>>(pParentNode)
{
	tinyxml2::XMLElement *pNode = pParentNode->FirstChildElement(XML_TAG_Value);
	loadChildren<CLinkConnection>(pNode, XML_TAG_LinkConnection, m_value);
}