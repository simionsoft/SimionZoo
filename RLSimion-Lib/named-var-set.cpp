#include "stdafx.h"
#include "named-var-set.h"

CNamedVarProperties::CNamedVarProperties(const char* name, const char* units, double min, double max, bool bCircular)
{
	sprintf_s(m_name, VAR_NAME_MAX_LENGTH, name);
	sprintf_s(m_units, VAR_NAME_MAX_LENGTH, units);
	m_min = min;
	m_max = max;
	m_bCircular = bCircular; //default value
}

void CNamedVarProperties::setName(const char* name)
{
	strcpy_s(m_name, VAR_NAME_MAX_LENGTH, name);
}

int CDescriptor::getVarIndex(const char* name)
{
	for (unsigned int i = 0; i<m_pProperties.size(); i++)
	{
		if (strcmp(m_pProperties[i]->getName(), name) == 0)
			return i;
	}
	return -1; //error return value
}

int CDescriptor::addVariable(const char* name, const char* units, double min, double max, bool bCircular)
{
	int index = (int) m_pProperties.size();
	m_pProperties.push_back(new CNamedVarProperties(name, units, min, max, bCircular));
	return index;
}

CNamedVarSet* CDescriptor::getInstance()
{
	CNamedVarSet* pNew = new CNamedVarSet(*this);

	return pNew;
}

CNamedVarSet::CNamedVarSet(CDescriptor& descriptor): m_pProperties(descriptor)
{

	//m_pProperties= new CNamedVarProperties[numVars];
	m_pValues= new double[descriptor.size()];
	for (unsigned int i = 0; i < descriptor.size(); i++) m_pValues[i] = 0.0;
	m_numVars= (int)descriptor.size();
}

CNamedVarSet::~CNamedVarSet()
{
	if (m_pValues) delete [] m_pValues;
}

double CNamedVarSet::get(const char* varName) const
{
	int varIndex = m_pProperties.getVarIndex(varName);
	if (varIndex >= 0)
		return m_pValues[varIndex];

	return 0.0;
}

int CNamedVarSet::getVarIndex(const char* varName) const
{
	return m_pProperties.getVarIndex(varName);
}

CNamedVarProperties& CNamedVarSet::getProperties(const char* varName) const
{
	int varIndex = m_pProperties.getVarIndex(varName);
	return m_pProperties[varIndex];
}

void CNamedVarSet::set(const char* varName, double value)
{
	int i = m_pProperties.getVarIndex(varName);
	if (i >= 0)
	{
		set(i, value);
		return;
	}
	
	assert(0);
}

double CNamedVarSet::get(int i) const
{
	if (i>=0 && i<m_numVars)
		return m_pValues[i];
	assert(0);
	return 0.0;
}

double* CNamedVarSet::getValuePtr(int i)
{
	if (i >= 0 && i<m_numVars)
		return &m_pValues[i];
	assert(0);
	return 0;
}

void CNamedVarSet::set(int i, double value)
{
	if (i >= 0 && i < m_numVars)
	{
		if (!m_pProperties[i].bIsCircular())
		{
			m_pValues[i] = std::min(m_pProperties[i].getMax()
				, std::max(m_pProperties[i].getMin(), value));
		}
		else
		{
			if (value > m_pProperties[i].getMax())
				value -= m_pProperties[i].getRangeWidth();
			else if (value < m_pProperties[i].getMin())
				value += m_pProperties[i].getRangeWidth();
			m_pValues[i] = value;
		}
	}
}

double CNamedVarSet::getSumValue() const
{
	double sum = 0.0;
	for (int i = 0; i < m_numVars; i++)
		sum += m_pValues[i];
	return sum;
}

void CNamedVarSet::copy(CNamedVarSet* nvs)
{
	assert(m_numVars==nvs->getNumVars());

	for (int i= 0; i<m_numVars; i++)
	{
		set(i,nvs->get(i));
	}
}

void CNamedVarSet::addOffset(double offset)
{
	for (int i = 0; i<m_numVars; i++)
	{
		set(i, this->get(i) + offset);
	}
}