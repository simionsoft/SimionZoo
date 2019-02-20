#include "parameters.h"
#include "../Common/named-var-set.h"
#include "app.h"
#define WIRE_XML_TAG "Wire"
#define WIRE_XML_MAX_ATTRIBUTE "Max"
#define WIRE_XML_MIN_ATTRIBUTE "Min"

STATE_VARIABLE::STATE_VARIABLE(ConfigNode* pConfigNode, const char* name, const char* comment)
{
	ConfigNode* pWiredChild = pConfigNode->getChild(name)->getChild(WIRE_XML_TAG);
	if (pWiredChild == nullptr)
	{
		m_variableName = pConfigNode->getConstString(name);
		m_name = name;
		m_comment = comment;
	}
	else
	{
		m_variableName = pWiredChild->GetText();
		if (pWiredChild->Attribute(WIRE_XML_MIN_ATTRIBUTE) == nullptr
			|| pWiredChild->Attribute(WIRE_XML_MAX_ATTRIBUTE) == nullptr)
			SimionApp::get()->wireRegister(m_variableName);
		else
		{
			double minimum = atof(pWiredChild->Attribute(WIRE_XML_MIN_ATTRIBUTE));
			double maximum = atof(pWiredChild->Attribute(WIRE_XML_MAX_ATTRIBUTE));
			SimionApp::get()->wireRegister(m_variableName, minimum, maximum);
		}
		m_name = name;
		m_comment = comment;
	}
}

STATE_VARIABLE::STATE_VARIABLE(const char* variableName)
{
	m_variableName = variableName;
	m_name = "State-variable";
	m_comment = "Object created from code, not a data file";
}

ACTION_VARIABLE::ACTION_VARIABLE(ConfigNode* pConfigNode, const char* name, const char* comment)
{
	ConfigNode* pWiredChild = pConfigNode->getChild(name)->getChild(WIRE_XML_TAG);
	if (pWiredChild == nullptr)
	{
		m_variableName = pConfigNode->getConstString(name);
		m_name = name;
		m_comment = comment;
	}
	else
	{
		m_variableName = pWiredChild->GetText();
		if (pWiredChild->Attribute(WIRE_XML_MIN_ATTRIBUTE) == nullptr
			|| pWiredChild->Attribute(WIRE_XML_MAX_ATTRIBUTE) == nullptr)
			SimionApp::get()->wireRegister(m_variableName);
		else
		{
			double minimum = atof(pWiredChild->Attribute(WIRE_XML_MIN_ATTRIBUTE));
			double maximum = atof(pWiredChild->Attribute(WIRE_XML_MAX_ATTRIBUTE));
			SimionApp::get()->wireRegister(m_variableName, minimum, maximum);
		}
		m_name = name;
		m_comment = comment;
	}
}

ACTION_VARIABLE::ACTION_VARIABLE(const char* variableName)
{
	m_variableName = variableName;
	m_name = "Action-variable";
	m_comment = "Object created from code, not a data file";
}

#include "app.h"
#include "../Common/wire.h"

WIRE_CONNECTION::WIRE_CONNECTION(ConfigNode* pConfigNode, const char* name, const char* comment)
{
	m_name = pConfigNode->getConstString(name);
	m_comment = comment;
}

WIRE_CONNECTION::WIRE_CONNECTION(const char* wireName)
{
	m_name = wireName;
}

double WIRE_CONNECTION::get()
{
	Wire* pWire = SimionApp::get()->wireGet(m_name);
	return pWire->getValue();
}
void WIRE_CONNECTION::set(double value)
{
	Wire* pWire = SimionApp::get()->wireGet(m_name);
	pWire->setValue(value);
}
#if defined(__linux__) || defined(_WIN64)
	#include "../CNTKWrapper/CNTKWrapper.h"
#endif
//Overriden Copy-assignment to avoid destroying copied buffer
//Expected use: problem= NN_DEFINITION(...)

NN_DEFINITION&  NN_DEFINITION::operator=(const NN_DEFINITION& copied)
{
	//we move pointers to the copy
	m_pDefinition = copied.m_pDefinition;

	//copied.m_pDefinition = nullptr; //cannot (const) and need not do this (destructor doesn't call destroy())

	m_name = copied.m_name;
	m_comment = copied.m_comment;

	return *this;
}

NN_DEFINITION::~NN_DEFINITION()
{
}

NN_DEFINITION::NN_DEFINITION(ConfigNode* pConfigNode, const char* name, const char* comment)
{
	m_name = name;
	m_comment = comment;
#if defined(_WIN64) || defined(__linux__)
	m_pDefinition = CNTK::WrapperClient::getNetworkDefinition(pConfigNode->FirstChildElement(m_name)->FirstChildElement("Problem"));
#endif
}

void NN_DEFINITION::destroy()
{
#if defined(_WIN64) || defined(__linux__)
	if (m_pDefinition)
	{
		m_pDefinition->destroy();
		m_pDefinition = nullptr;
	}
#endif
}