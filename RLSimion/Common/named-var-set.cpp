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

#include "named-var-set.h"
#include "wire.h"
#include "wire-handler.h"
#include <algorithm>
#include <stdexcept>

#include "../../tools/System/CrossPlatform.h"

using namespace std;

NamedVarProperties::NamedVarProperties()
{
	CrossPlatform::Strcpy_s(m_name, VAR_NAME_MAX_LENGTH, "N/A");
	CrossPlatform::Strcpy_s(m_units, VAR_NAME_MAX_LENGTH, "N/A");
	m_min = std::numeric_limits<double>::lowest();
	m_max = std::numeric_limits<double>::max(); 
}

NamedVarProperties::NamedVarProperties(const char* name, const char* units, double min, double max, bool bCircular)
{
	CrossPlatform::Sprintf_s(m_name, VAR_NAME_MAX_LENGTH, name);
	CrossPlatform::Sprintf_s(m_units, VAR_NAME_MAX_LENGTH, units);
	m_min = min;
	m_max = max;
	m_bCircular = bCircular; //default value
}

void NamedVarProperties::setName(const char* name)
{
	CrossPlatform::Strcpy_s(m_name, VAR_NAME_MAX_LENGTH, name);
}

NamedVarProperties* Descriptor::getProperties(const char* name)
{
	for (size_t i = 0; i<m_descriptor.size(); i++)
	{
		if (strcmp(m_descriptor[i]->getName(), name) == 0)
			return m_descriptor[i];
	}
	//check if a wire with that name exists
	Wire* pWire = m_pWireHandler->wireGet(name);
	if (pWire != nullptr)
	{
		NamedVarProperties* pProperties = pWire->getProperties();
		if (pProperties==nullptr)
			return &wireDummyProperties;
		return pProperties;
	}
	else
		throw std::runtime_error("Wrong variable name given to Descriptor::getVarIndex()");
}

size_t Descriptor::addVariable(const char* name, const char* units, double min, double max, bool bCircular)
{
	size_t index = (int) m_descriptor.size();
	m_descriptor.push_back(new NamedVarProperties(name, units, min, max, bCircular));
	return index;
}

NamedVarSet* Descriptor::getInstance()
{
	NamedVarSet* pNew = new NamedVarSet(*this);

	return pNew;
}

NamedVarSet::NamedVarSet(Descriptor& descriptor): m_descriptor(descriptor)
{

	//m_descriptor= new NamedVarProperties[numVars];
	m_pValues= new double[descriptor.size()];
	for (size_t i = 0; i < descriptor.size(); i++)
		m_pValues[i] = 0.0;
	m_numVars= (int)descriptor.size();
}

NamedVarSet::~NamedVarSet()
{
	if (m_pValues) delete [] m_pValues;
}


NamedVarProperties* NamedVarSet::getProperties(const char* varName) const
{
	return m_descriptor.getProperties(varName);
}

double NamedVarSet::normalize(const char* varName, double value) const
{
	NamedVarProperties* pProperties = getProperties(varName);
	double range = std::max(0.01, pProperties->getRangeWidth());
	return (value - pProperties->getMin()) / range;
}

double NamedVarSet::denormalize(const char* varName, double value) const
{
	NamedVarProperties* pProperties = getProperties(varName);
	double range = pProperties->getRangeWidth();
	return pProperties->getMin() + value * range;
}

void NamedVarSet::setNormalized(const char* varName, double value)
{
	double denormalizedValue = denormalize(varName, value);
	set(varName, denormalizedValue);
}


void NamedVarSet::set(const char* varName, double value)
{
	for (size_t i = 0; i < m_descriptor.size(); i++)
	{
		if (!strcmp(m_descriptor[i].getName(), varName))
		{
			set(i, value);
			return;
		}
	}

	//check if a wire with that name exists
	WireHandler* pWireHandler = m_descriptor.getWireHandler();
	if (pWireHandler != nullptr)
	{
		Wire* pWire = pWireHandler->wireGet(varName);
		if (pWire != nullptr)
			pWire->setValue(value);
		else
			throw std::runtime_error("Incorrect variable name in NamedVarSet::set()");
	}
	else
		throw std::runtime_error("Incorrect variable name in NamedVarSet::set()");
}

void NamedVarSet::set(size_t i, double value)
{
	if (i >= 0 && i < m_numVars)
	{
		if (!m_descriptor[i].isCircular())
		{
			m_pValues[i] = std::min(m_descriptor[i].getMax()
				, std::max(m_descriptor[i].getMin(), value));
		}
		else
		{
			if (value > m_descriptor[i].getMax())
				value -= m_descriptor[i].getRangeWidth();
			else if (value < m_descriptor[i].getMin())
				value += m_descriptor[i].getRangeWidth();
			m_pValues[i] = value;
		}
	}
	else throw std::runtime_error("Incorrect variable index in NamedVarSet::set()");
}

double NamedVarSet::getNormalized(const char* varName) const
{
	return normalize(varName, get(varName));
}

double NamedVarSet::get(size_t i) const
{
	if (i >= 0 && i<m_numVars)
		return m_pValues[i];
	throw std::runtime_error("Incorrect variable index in NamedVarSet::get()");
}

double NamedVarSet::get(const char* varName) const
{
	for (size_t i = 0; i < m_descriptor.size(); i++)
	{
		if (!strcmp(m_descriptor[i].getName(), varName))
			return m_pValues[i];
	}

	WireHandler* pWireHandler = m_descriptor.getWireHandler();
	if (pWireHandler != nullptr)
	{
		Wire* pWire= pWireHandler->wireGet(varName);
		if (pWire != nullptr)
			return pWire->getValue();
		else
			throw std::runtime_error("Incorrect variable name in NamedVarSet::get()");
	}
	else
		throw std::runtime_error("Incorrect variable name in NamedVarSet::get()");
}


double* NamedVarSet::getValuePtr(size_t i)
{
	if (i >= 0 && i<m_numVars)
		return &m_pValues[i];
	throw std::runtime_error("Incorrect variable index in NamedVarSet::getValuePtr()");
}

double& NamedVarSet::getRef(size_t i)
{
	if (i >= 0 && i<m_numVars)
		return m_pValues[i];
	throw std::runtime_error("Incorrect variable index in NamedVarSet::getRef()");
}



double NamedVarSet::getSumValue() const
{
	double sum = 0.0;
	for (size_t i = 0; i < m_numVars; i++)
		sum += m_pValues[i];
	return sum;
}

void NamedVarSet::copy(const NamedVarSet* nvs)
{
	if(m_numVars != nvs->getNumVars())
		throw std::runtime_error("Missmatched array lenghts in NamedVarSet::copy()");

	for (size_t i = 0; i<m_numVars; i++)
	{
		set(i, nvs->get(i));
	}
}

void NamedVarSet::addOffset(double offset)
{
	for (size_t i = 0; i<m_numVars; i++)
	{
		set(i, this->get(i) + offset);
	}
}