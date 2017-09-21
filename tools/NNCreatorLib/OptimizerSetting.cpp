#include "stdafx.h"
#include "OptimizerSetting.h"
#include "xmltags.h"
#include "Exceptions.h"

COptimizerSetting::COptimizerSetting(tinyxml2::XMLElement * pParentNode)
{
	const char* type = pParentNode->Attribute(XML_ATTRIBUTE_Type);
	if (!strcmp(type, "OptimizerSGD"))
		m_optimizerType = OptimizerType::SGD;
	else if (!strcmp(type, "OptimizerMomentumSGD"))
		m_optimizerType = OptimizerType::MomentumSGD;
	else if (!strcmp(type, "OptimizerNesterov"))
		m_optimizerType = OptimizerType::Nesterov;
	else if (!strcmp(type, "OptimizerFSAdaGrad"))
		m_optimizerType = OptimizerType::FSAdaGrad;
	else if (!strcmp(type, "OptimizerAdam"))
		m_optimizerType = OptimizerType::Adam;
	else if (!strcmp(type, "OptimizerAdaGrad"))
		m_optimizerType = OptimizerType::AdaGrad;
	else if (!strcmp(type, "OptimizerRMSProp"))
		m_optimizerType = OptimizerType::RMSProp;
	else if (!strcmp(type, "OptimizerAdaDelta"))
		m_optimizerType = OptimizerType::AdaDelta;
	else
		throw ProblemParserElementValueNotValid(XML_ATTRIBUTE_Type);

	tinyxml2::XMLElement *pNode = pParentNode->FirstChildElement(XML_TAG_Parameters);
	if (pNode == nullptr)
		throw ProblemParserElementNotFound(XML_TAG_Parameters);
	loadChildren<COptimizerParameter>(pNode, XML_TAG_OptimizerParameter, m_parameters);
}

COptimizerSetting::COptimizerSetting()
{
}


COptimizerSetting::~COptimizerSetting()
{
}

COptimizerSetting * COptimizerSetting::getInstance(tinyxml2::XMLElement * pNode)
{
	return new COptimizerSetting(pNode);
}

const COptimizerParameter * COptimizerSetting::getParameterByKey(std::string key) const
{
	for each (auto pParameter in m_parameters)
	{
		if (pParameter->getKey() == key)
		{
			return pParameter;
		}
	}

	return nullptr;
}

COptimizerParameter::COptimizerParameter(tinyxml2::XMLElement * pParentNode)
{
	tinyxml2::XMLElement *pNode = pParentNode->FirstChildElement(XML_TAG_Value);
	if (pNode == nullptr)
		throw ProblemParserElementNotFound(XML_TAG_Value);
	m_value = atof(pNode->GetText());

	pNode = pParentNode->FirstChildElement(XML_TAG_Key);
	if (pNode == nullptr)
		throw ProblemParserElementNotFound(XML_TAG_Key);
	m_key = pNode->GetText();
}

COptimizerParameter * COptimizerParameter::getInstance(tinyxml2::XMLElement * pNode)
{
	if (!strcmp(pNode->Name(), XML_TAG_OptimizerParameter))
		return new COptimizerParameter(pNode);
	return nullptr;
}
