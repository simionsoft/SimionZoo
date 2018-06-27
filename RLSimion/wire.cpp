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

string Wire::getName()
{
	return m_name;
}

double Wire::getValue()
{
	return m_value;
}

void Wire::setValue(double value)
{
	if (!m_bPropertiesSet)
		m_value = value;
	else
		m_value = std::min(m_properties.getMax(), std::max(m_properties.getMin(), value));
}

void Wire::setLimits(double minimum, double maximum)
{
	m_bPropertiesSet = true;
	m_properties.setMin(minimum);
	m_properties.setMax(maximum);
}