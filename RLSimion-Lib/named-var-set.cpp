#include "stdafx.h"
#include "named-var-set.h"

CNamedVarProperties::CNamedVarProperties(const char* name, const char* units, double min, double max)
{
	sprintf_s(m_name, VAR_NAME_MAX_LENGTH, name);
	sprintf_s(m_units, VAR_NAME_MAX_LENGTH, units);
	m_min = min;
	m_max = max;
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

int CDescriptor::addVariable(const char* name, const char* units, double min, double max)
{
	int index = m_pProperties.size();
	m_pProperties.push_back(new CNamedVarProperties(name, units, min, max));
	return index;
}

CNamedVarSet* CDescriptor::getInstance()
{
	int numVars = m_pProperties.size();
	CNamedVarSet* pNew = new CNamedVarSet(*this);

	return pNew;
}

CNamedVarSet::CNamedVarSet(CDescriptor& descriptor): m_pProperties(descriptor)
{

	//m_pProperties= new CNamedVarProperties[numVars];
	m_pValues= new double[descriptor.size()];
	m_numVars= descriptor.size();
}

CNamedVarSet::~CNamedVarSet()
{
	if (m_pValues) delete [] m_pValues;
}

double CNamedVarSet::getValue(const char* varName) const
{
	int varIndex = m_pProperties.getVarIndex(varName);
	if (varIndex >= 0)
		return m_pValues[varIndex];

	assert(0);
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

void CNamedVarSet::setValue(const char* varName, double value)
{
	int i = m_pProperties.getVarIndex(varName);
	if (i >= 0) setValue(i, value);
	
	assert(0);
}

double CNamedVarSet::getValue(int i) const
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

void CNamedVarSet::setValue(int i, double value)
{
	if (i >= 0 && i < m_numVars)
	{
		m_pValues[i] = std::min(m_pProperties[i].getMax()
			, std::max(m_pProperties[i].getMin(), value));
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
		setValue(i,nvs->getValue(i));
	}
}
