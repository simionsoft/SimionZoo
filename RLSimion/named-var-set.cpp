#include "named-var-set.h"
#include <algorithm>
using namespace std;

NamedVarProperties::NamedVarProperties(const char* name, const char* units, double min, double max, bool bCircular)
{
	sprintf_s(m_name, VAR_NAME_MAX_LENGTH, name);
	sprintf_s(m_units, VAR_NAME_MAX_LENGTH, units);
	m_min = min;
	m_max = max;
	m_bCircular = bCircular; //default value
}

void NamedVarProperties::setName(const char* name)
{
	strcpy_s(m_name, VAR_NAME_MAX_LENGTH, name);
}

int Descriptor::getVarIndex(const char* name)
{
	for (unsigned int i = 0; i<m_descriptor.size(); i++)
	{
		if (strcmp(m_descriptor[i]->getName(), name) == 0)
			return i;
	}
	return -1; //error return value
}

int Descriptor::addVariable(const char* name, const char* units, double min, double max, bool bCircular)
{
	int index = (int) m_descriptor.size();
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
	for (unsigned int i = 0; i < descriptor.size(); i++) m_pValues[i] = 0.0;
	m_numVars= (int)descriptor.size();
}

NamedVarSet::~NamedVarSet()
{
	if (m_pValues) delete [] m_pValues;
}

int NamedVarSet::getVarIndex(const char* varName) const
{
	return m_descriptor.getVarIndex(varName);
}

NamedVarProperties& NamedVarSet::getProperties(const char* varName) const
{
	int varIndex = m_descriptor.getVarIndex(varName);
	return m_descriptor[varIndex];
}

double NamedVarSet::normalize(int i, double value) const
{
	NamedVarProperties& properties = getProperties(i);
	double range = std::max(0.01, properties.getRangeWidth());
	return (value - properties.getMin()) / range;
}

double NamedVarSet::denormalize(int i, double value) const
{
	NamedVarProperties& properties = getProperties(i);
	double range = properties.getRangeWidth();
	return properties.getMin() + value * range;
}

void NamedVarSet::setNormalized(const char* varName, double value)
{
	int i = m_descriptor.getVarIndex(varName);
	if (i >= 0)
	{
		setNormalized(i, value);
		return;
	}
	throw std::exception("Incorrect variable name in NamedVarSet::set()");
}

void NamedVarSet::setNormalized(int i, double value)
{
	set(i, denormalize(i, value));
}

void NamedVarSet::set(const char* varName, double value)
{
	int i = m_descriptor.getVarIndex(varName);
	if (i >= 0)
	{
		set(i, value);
		return;
	}
	throw std::exception("Incorrect variable name in NamedVarSet::set()");
}

void NamedVarSet::set(int i, double value)
{
	if (i >= 0 && i < m_numVars)
	{
		if (!m_descriptor[i].bIsCircular())
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
	throw std::exception("Incorrect variable index in NamedVarSet::set()");

}

double NamedVarSet::getNormalized(const char* varName) const
{
	int varIndex = m_descriptor.getVarIndex(varName);
	if (varIndex >= 0)
		return normalize(varIndex, m_pValues[varIndex]);
	throw std::exception("Incorrect variable index in NamedVarSet::getNormalized()");
}

double NamedVarSet::getNormalized(int i) const
{
	if (i >= 0 && i<m_numVars)
		return normalize(i, m_pValues[i]);
	throw std::exception("Incorrect variable index in NamedVarSet::getNormalized()");
}

double NamedVarSet::get(int i) const
{
	if (i >= 0 && i<m_numVars)
		return m_pValues[i];
	throw std::exception("Incorrect variable index in NamedVarSet::get()");
}

double NamedVarSet::get(const char* varName) const
{
	int varIndex = m_descriptor.getVarIndex(varName);
	if (varIndex >= 0)
		return m_pValues[varIndex];
	throw std::exception("Incorrect variable index in NamedVarSet::get()");
}


double* NamedVarSet::getValuePtr(int i)
{
	if (i >= 0 && i<m_numVars)
		return &m_pValues[i];
	throw std::exception("Incorrect variable index in NamedVarSet::getValuePtr()");
}

double& NamedVarSet::getRef(int i)
{
	if (i >= 0 && i<m_numVars)
		return m_pValues[i];
	throw std::exception("Incorrect variable index in NamedVarSet::getRef()");
}



double NamedVarSet::getSumValue() const
{
	double sum = 0.0;
	for (int i = 0; i < m_numVars; i++)
		sum += m_pValues[i];
	return sum;
}

void NamedVarSet::copy(const NamedVarSet* nvs)
{
	if(m_numVars != nvs->getNumVars())
		throw std::exception("Missmatched array lenghts in NamedVarSet::copy()");

	for (int i = 0; i<m_numVars; i++)
	{
		set(i, nvs->get(i));
	}
}

void NamedVarSet::addOffset(double offset)
{
	for (int i = 0; i<m_numVars; i++)
	{
		set(i, this->get(i) + offset);
	}
}