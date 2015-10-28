#include "stdafx.h"
#include "states-and-actions.h"




CNamedVarSet::CNamedVarSet(int numVars)
{
	m_pProperties= new CNamedVarProperties[numVars];
	m_pValues= new double[numVars];
	m_numVars= numVars;
}

CNamedVarSet::~CNamedVarSet()
{
	if (m_pProperties) delete [] m_pProperties;
	if (m_pValues) delete [] m_pValues;
}


char* CNamedVarSet::getName(int i)
{
	if (i>=0 && i<m_numVars)
		return m_pProperties[i].name;
	return 0;
}
double CNamedVarSet::getMin(int i)
{
	if (i>=0 && i<m_numVars)
		return m_pProperties[i].min;
	return 0.0;
}
double CNamedVarSet::getMax(int i)
{
	if (i>=0 && i<m_numVars)
		return m_pProperties[i].max;
	return 0.0;
}
double CNamedVarSet::getMin(char* name)
{
	int i= getVarIndex(name);
	if (i>=0 && i<m_numVars)
		return m_pProperties[i].min;
	return 0.0;
}
double CNamedVarSet::getMax(char* name)
{
	int i= getVarIndex(name);
	if (i>=0 && i<m_numVars)
		return m_pProperties[i].max;
	return 0.0;
}

void CNamedVarSet::setName(int i,char* name)
{
	if (i>=0 && i<m_numVars)
		strcpy_s(m_pProperties[i].name,VAR_NAME_MAX_LENGTH,name);
}
void CNamedVarSet::setMin(int i,double min)
{
	if (i>=0 && i<m_numVars)
		m_pProperties[i].min= min;
}
void CNamedVarSet::setMax(int i,double max)
{
	if (i>=0 && i<m_numVars)
		m_pProperties[i].max= max;
}
void CNamedVarSet::setProperties(int i,char* name, double min, double max)
{
	if (i>=0 && i<m_numVars)
	{
		strcpy_s(m_pProperties[i].name,VAR_NAME_MAX_LENGTH,name);
		m_pProperties[i].min= min;
		m_pProperties[i].max= max;
	}
}


double CNamedVarSet::getValue(char* varName)
{
	for (int i= 0; i<m_numVars; i++)
	{
		if (strcmp(m_pProperties[i].name,varName)==0)
			return m_pValues[i];
	}
	assert(0);
	return 0.0;
}

int CNamedVarSet::getVarIndex(char* varName)
{
	for (int i= 0; i<m_numVars; i++)
	{
		if (strcmp(m_pProperties[i].name,varName)==0)
			return i;
	}
	return -1;
}

void CNamedVarSet::setValue(char* varName, double value)
{
	for (int i= 0; i<m_numVars; i++)
	{
		if (strcmp(m_pProperties[i].name,varName)==0)
		{
			m_pValues[i]= std::min(m_pProperties[i].max,std::max(m_pProperties[i].min,value));
			return;
		}
	}
	assert(0);
}

double CNamedVarSet::getValue(int i)
{
	if (i>=0 && i<m_numVars)
		return m_pValues[i];
	assert(0);
	return 0.0;
}

void CNamedVarSet::setValue(int i, double value)
{
	if (i>=0 && i<m_numVars)
		m_pValues[i]= std::min(m_pProperties[i].max,std::max(m_pProperties[i].min,value));
}

void CNamedVarSet::copy(CNamedVarSet* nvs)
{
	assert(m_numVars==nvs->getNumVars());

	for (int i= 0; i<m_numVars; i++)
	{
		setValue(i,nvs->getValue(i));
	}
}

CNamedVarSet* CNamedVarSet::getInstance()
{
	CNamedVarSet* pNew= new CNamedVarSet(m_numVars);

	for (int i=0; i<m_numVars; i++)
	{
		pNew->setName(i,this->getName(i));
		pNew->setMin(i,this->getMin(i));
		pNew->setMax(i,this->getMax(i));
		pNew->setValue(i,this->getValue(i));
	}
	return pNew;
}