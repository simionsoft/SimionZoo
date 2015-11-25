#include "stdafx.h"
#include "parameter.h"


//CPARAMETER
CParameter::CParameter(char* name, char* stringValue)
{
	setName(name);
	setValue(stringValue);
}
CParameter::CParameter(char* name, double value)
{
	setName(name);
	setValue(value);
}


CParameter::CParameter()
{
	name[0] = 0;
	pValue = 0;
	type = NOT_INITIALIZED;
}

void CParameter::releasePtr()
{
	if (pValue && type != NOT_INITIALIZED)
	{
		if (type == NUMERIC_PARAMETER)
		{
			delete (double*)pValue;
			pValue = 0;
		}
		if (type == STRING_PARAMETER)
		{
			delete[](char*) pValue;
		}
	}
	type = NOT_INITIALIZED;
}

CParameter::~CParameter()
{
	releasePtr();
}

CParameter& CParameter::operator= (const CParameter& parameter)
{
	releasePtr();

	type = parameter.type;
	strcpy_s(name, MAX_PARAMETER_NAME_SIZE, parameter.name);

	if (type == NUMERIC_PARAMETER)
	{
		pValue = new double;
		*(double*)pValue = *(double*)parameter.pValue;
	}
	else if (type == STRING_PARAMETER)
	{
		pValue = new char[MAX_STRING_SIZE];
		strcpy_s((char*)pValue, MAX_STRING_SIZE, (char*)parameter.pValue);
	}

	return *this;
}

bool CParameter::operator== (const CParameter& parameter)
{
	if (strcmp(name, parameter.name)) return false;

	return true;
}

const char* CParameter::getName() const
{
	return name;
}
void CParameter::setName(char* pName)
{
	strcpy_s(name, MAX_PARAMETER_NAME_SIZE, pName);
}
bool CParameter::nameIs(const char* pName) const
{
	return strcmp(name, pName) == 0;
}
void CParameter::setValue(char* string)
{
	releasePtr();
	pValue = new char[MAX_STRING_SIZE];
	strcpy_s((char*)pValue, MAX_STRING_SIZE, string);
	type = STRING_PARAMETER;
}
void CParameter::setValue(double value)
{
	releasePtr();
	pValue = new char[MAX_STRING_SIZE];
	*(double*)pValue = value;
	type = NUMERIC_PARAMETER;
}

const char* CParameter::getStringPtr() const
{
	assert(type == STRING_PARAMETER);
	return (char*)pValue;
}

double CParameter::getDouble() const
{
	assert(type == NUMERIC_PARAMETER);
	return *(double*)pValue;
}
void CParameter::saveParameter(void* pFile, char* prefix) const
{
	if (prefix)
	{
		if (type == NUMERIC_PARAMETER)
			fprintf((FILE*)pFile, "%s/%s : %f\n", prefix, name, *(double*)pValue);
		else
			fprintf((FILE*)pFile, "%s/%s : %s\n", prefix, name, (char*)pValue);
	}
	else
	{
		if (type == NUMERIC_PARAMETER)
			fprintf((FILE*)pFile, "%s : %f\n", name, *(double*)pValue);
		else
			fprintf((FILE*)pFile, "%s : %s\n", name, (char*)pValue);
	}
}