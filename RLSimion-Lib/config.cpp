#include "stdafx.h"
#include "config.h"
#include "globals.h"
#include "experiment.h"
#include "logger.h"


CConfigNode* CConfigFile::loadFile(const char* fileName, const char* nodeName)
{
	LoadFile(fileName);
	if (Error()) throw std::exception((std::string("Couldn't load file: ") + std::string(fileName)).c_str());

	if (nodeName)
		return (CConfigNode*) (this->FirstChildElement(nodeName));
	return (CConfigNode*)this->FirstChildElement();
	
}

const char* CConfigFile::getError()
{
	return ErrorName();
}


int CConfigNode::countChildren(const char* name)
{
	int count = 0;
	CConfigNode* p;

	if (!this)
		throw std::exception((std::string("Illegal count of children parameters")).c_str());
	
	if (name) p= getChild(name);
	else p = getChild();

	while (p != 0)
	{
		count++;

		if (name)
			p = p->getNextSibling(name);
		else p = p->getNextSibling();
	}
	return count;
}



bool CConfigNode::getConstBoolean(const char* paramName, bool defaultValue)
{
	tinyxml2::XMLElement* pParameter;

	if (!this)
		throw std::exception((std::string("Illegal access to boolean parameter") + std::string(paramName)).c_str());

	if (paramName) pParameter = getChild(paramName);
	else pParameter = this;

	if (pParameter)
	{
		if (!strcmp(pParameter->GetText(), "true"))
			return true;
		if (!strcmp(pParameter->GetText(), "false"))
			return false;
	}
	else throw std::exception((std::string("Illegal access to boolean parameter") + std::string(paramName)).c_str());

	char msg[128];
	sprintf_s(msg, 128, "Parameter %s/%s not found. Using default value %d", getName(), paramName, defaultValue);
	CLogger::logMessage(Warning, msg);

	return defaultValue;
}

int CConfigNode::getConstInteger(const char* paramName, int defaultValue)
{
	tinyxml2::XMLElement* pParameter;
	if (!this)
		throw std::exception((std::string("Illegal access to integer parameter") + std::string(paramName)).c_str());

	if (paramName) pParameter = getChild(paramName);
	else pParameter = this;

	if (pParameter)
	{
		return atoi(pParameter->GetText());
	}
	else throw std::exception((std::string("Illegal access to boolean parameter") + std::string(paramName)).c_str());

	char msg[128];
	sprintf_s(msg, 128, "Parameter %s/%s not found. Using default value %d", getName(), paramName, defaultValue);
	CLogger::logMessage(Warning, msg);
	return defaultValue;
}

double CConfigNode::getConstDouble(const char* paramName, double defaultValue)
{
	CConfigNode* pParameter;
	if (!this)
		throw std::exception((std::string("Illegal access to double parameter") + std::string(paramName)).c_str());

	if (paramName) pParameter = getChild(paramName);
	else pParameter = this;

	if (pParameter)
	{
		return atof(pParameter->GetText());
	}
	else throw std::exception((std::string("Illegal access to double parameter") + std::string(paramName)).c_str());

	char msg[128];
	sprintf_s(msg, 128, "Parameter %s/%s not found. Using default value %f", getName(), paramName, defaultValue);
	CLogger::logMessage(Warning, msg);

	return defaultValue;
}

const char* CConfigNode::getConstString(const char* paramName, const char* defaultValue)
{
	CConfigNode* pParameter;
	if (!this)
		throw std::exception((std::string("Illegal access to string parameter") + std::string(paramName)).c_str());

	if (paramName) pParameter = getChild(paramName);
	else pParameter = this;

	if (pParameter)
	{
		return pParameter->GetText();
	}
	else throw std::exception((std::string("Illegal access to boolean parameter") + std::string(paramName)).c_str());

	char msg[128];
	sprintf_s(msg, 128, "Parameter %s/%s not found. Using default value %s", getName(), paramName, defaultValue);
	CLogger::logMessage(Warning, msg);

	return defaultValue;
}

CConfigNode* CConfigNode::getChild(const char* paramName)
{
	if (!this)
		throw std::exception("Illegal access to child parameter ");

	tinyxml2::XMLElement* child = FirstChildElement(paramName);
	return static_cast<CConfigNode*> (child);
}

CConfigNode* CConfigNode::getNextSibling(const char* paramName)
{
	if (!this)
		throw std::exception("Illegal access to child parameter ");

	tinyxml2::XMLElement* child = NextSiblingElement(paramName);
	return static_cast<CConfigNode*> (child);
}

const char* CConfigNode::getName()
{
	if (!this)
		throw std::exception("Illegal access to inexistant parameter's name");

	return Name();
}

void CConfigNode::saveFile(const char* pFilename)
{
	SaveFile(pFilename, false);
}

void CConfigNode::saveFile(FILE* pFile)
{
	SaveFile(pFile, false);
}

void CConfigNode::clone(CConfigFile* parameterFile)
{
	this->DeleteChildren();

	this->ShallowClone((tinyxml2::XMLDocument*) parameterFile);
}