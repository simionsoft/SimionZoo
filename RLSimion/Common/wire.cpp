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

#include "wire.h"
#include <algorithm>


Wire::Wire(string name)
{
	m_name = name;
	m_bPropertiesSet = false;
}

Wire::Wire(string name, double minimum, double maximum)
{
	m_name = name;
	m_properties = NamedVarProperties(name.c_str(), "N/A", minimum, maximum);
	m_bPropertiesSet = true;
}

Wire::~Wire()
{
}

NamedVarProperties* Wire::getProperties()
{
	if (m_bPropertiesSet)
		return &m_properties;
	return nullptr;
}

/// <summary>
/// Returns the name of the wire
/// </summary>
/// <returns>The name</returns>
string Wire::getName()
{
	return m_name;
}


/// <summary>
/// Returns the current value of the wire
/// </summary>
/// <returns>The wire's name</returns>
double Wire::getValue()
{
	return m_value;
}

/// <summary>
/// Sets the name of the wire
/// </summary>
/// <param name="value">New value given</param>
void Wire::setValue(double value)
{
	if (!m_bPropertiesSet)
		m_value = value;
	else
		m_value = std::min(m_properties.getMax(), std::max(m_properties.getMin(), value));
}


/// <summary>
/// Sets the value range of the wire
/// </summary>
/// <param name="minimum">Minimum value</param>
/// <param name="maximum">Maximum value</param>
void Wire::setLimits(double minimum, double maximum)
{
	m_bPropertiesSet = true;
	m_properties.setMin(minimum);
	m_properties.setMax(maximum);
}