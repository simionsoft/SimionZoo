#include "parameters.h"
#include "named-var-set.h"

STATE_VARIABLE::STATE_VARIABLE(ConfigNode* pConfigNode, const char* name, const char* comment)
{
	m_variableName = pConfigNode->getConstString(name);
	m_name = name;
	m_comment = comment;
}

STATE_VARIABLE::STATE_VARIABLE(const char* variableName)
{
	m_variableName = variableName;
	m_name = "State-variable";
	m_comment = "Object created from code, not a data file";
}

ACTION_VARIABLE::ACTION_VARIABLE(ConfigNode* pConfigNode, const char* name, const char* comment)
{
	m_variableName = pConfigNode->getConstString(name);
	m_name = name;
	m_comment = comment;
}

ACTION_VARIABLE::ACTION_VARIABLE(const char* variableName)
{
	m_variableName = variableName;
	m_name = "Action-variable";
	m_comment = "Object created from code, not a data file";
}

#include "app.h"

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
	return SimionApp::get()->wireGetValue(m_name);
}
void WIRE_CONNECTION::set(double value)
{
	return SimionApp::get()->wireSetValue(m_name, value);
}

#include "../tools/CNTKWrapper/CNTKWrapper.h"

//Overriden Copy-assignment to avoid destroying copied buffer
//Expected use: problem= NN_DEFINITION(...)
NN_DEFINITION&  NN_DEFINITION::operator=(NN_DEFINITION& copied)
{
	//we move pointers to the copy
	m_pDefinition = copied.m_pDefinition;

	copied.m_pDefinition = nullptr;

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
#ifdef _WIN64
	m_pDefinition = CNTK::WrapperClient::getNetworkDefinition(pConfigNode->FirstChildElement(m_name)->FirstChildElement("Problem"));
#endif
}

void NN_DEFINITION::destroy()
{
#ifdef _WIN64
	if (m_pDefinition)
	{
		m_pDefinition->destroy();
		m_pDefinition = nullptr;
	}
#endif
}