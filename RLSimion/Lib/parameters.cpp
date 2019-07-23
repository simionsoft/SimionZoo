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