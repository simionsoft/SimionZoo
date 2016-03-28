#include "stdafx.h"
#include "parameters.h"
#include "globals.h"
#include "experiment.h"
#include "logger.h"


#ifdef _DEBUG
#pragma comment (lib,"../Debug/tinyxml2.lib")
#else
#pragma comment (lib,"../Release/tinyxml2.lib")
#endif



CParameters* CParameterFile::loadFile(const char* fileName, const char* nodeName)
{
	LoadFile(fileName);
	if (Error()) throw std::exception((std::string("Couldn't load file: ") + std::string(fileName)).c_str());

	if (nodeName)
		return (CParameters*) (this->FirstChildElement(nodeName));
	return (CParameters*)this->FirstChildElement();
	
}

const char* CParameterFile::getError()
{
	return ErrorName();
}


int CParameters::countChildren(const char* name)
{
	int count = 0;
	CParameters* p;

	if (!this)
		throw std::exception((std::string("Illegal count of children parameters")).c_str());
	
	if (name) p= getChild(name);
	else p = getChild();

	while (p != 0)
	{
		count++;

		if (name)
			p = p->getNextChild(name);
		else p = p->getNextChild();
	}
	return count;
}



bool CParameters::getConstBoolean(const char* paramName, bool defaultValue)
{
	tinyxml2::XMLElement* pParameter;

	if (!this)
		throw std::exception((std::string("Illegal access to boolean parameter") + std::string(paramName)).c_str());

	pParameter = getChild(paramName);
	if (pParameter)
	{
		if (!strcmp(pParameter->GetText(), "true"))
			return true;
		if (!strcmp(pParameter->GetText(), "false"))
			return false;
	}
	char msg[128];
	sprintf_s(msg, 128, "Parameter %s/%s not found. Using default value %b", getName(), paramName, defaultValue);
	CLogger::logMessage(Warning, msg);

	return defaultValue;
}

int CParameters::getConstInteger(const char* paramName, int defaultValue)
{
	tinyxml2::XMLElement* pParameter;
	if (!this)
		throw std::exception((std::string("Illegal access to integer parameter") + std::string(paramName)).c_str());

	pParameter = getChild(paramName);
	if (pParameter)
	{
		return atoi(pParameter->GetText());
	}
	char msg[128];
	sprintf_s(msg, 128, "Parameter %s/%s not found. Using default value %d", getName(), paramName, defaultValue);
	CLogger::logMessage(Warning, msg);
	return defaultValue;
}

double CParameters::getConstDouble(const char* paramName, double defaultValue)
{
	CParameters* pParameter;
	if (!this)
		throw std::exception((std::string("Illegal access to double parameter") + std::string(paramName)).c_str());

	pParameter = getChild(paramName);
	if (pParameter)
	{
		return atof(pParameter->GetText());
	}
	char msg[128];
	sprintf_s(msg, 128, "Parameter %s/%s not found. Using default value %f", getName(), paramName, defaultValue);
	CLogger::logMessage(Warning, msg);

	return defaultValue;
}

const char* CParameters::getConstString(const char* paramName, const char* defaultValue)
{
	CParameters* pParameter;
	if (!this)
		throw std::exception((std::string("Illegal access to string parameter") + std::string(paramName)).c_str());

	if (paramName)
	{
		pParameter = getChild(paramName);
		if (pParameter)
		{
			return pParameter->GetText();
		}
	}
	else
		if (GetText()) return GetText();
	char msg[128];
	sprintf_s(msg, 128, "Parameter %s/%s not found. Using default value %s", getName(), paramName, defaultValue);
	CLogger::logMessage(Warning, msg);

	return defaultValue;
}

CParameters* CParameters::getChild(const char* paramName)
{
	if (!this)
		throw std::exception("Illegal access to child parameter");

	tinyxml2::XMLElement* child = FirstChildElement(paramName);
	return static_cast<CParameters*> (child);
}

CParameters* CParameters::getNextChild(const char* paramName)
{
	if (!this)
		throw std::exception("Illegal access to child parameter");

	tinyxml2::XMLElement* child = NextSiblingElement(paramName);
	return static_cast<CParameters*> (child);
}

const char* CParameters::getName()
{
	if (!this)
		throw std::exception("Illegal access to inexistant parameter's name");

	return Name();
}

void CParameters::saveFile(const char* pFilename)
{
	SaveFile(pFilename, false);
}

void CParameters::saveFile(FILE* pFile)
{
	SaveFile(pFile, false);
}

void CParameters::clone(CParameterFile* parameterFile)
{
	this->DeleteChildren();

	this->ShallowClone((tinyxml2::XMLDocument*) parameterFile);
}