#include "config.h"
#include "experiment.h"
#include "logger.h"


ConfigNode* ConfigFile::loadFile(const char* fileName, const char* nodeName)
{
	LoadFile(fileName);
	if (Error()) throw std::exception((std::string("Couldn't load file: ") + std::string(fileName)).c_str());

	if (nodeName)
		return (ConfigNode*) (this->FirstChildElement(nodeName));
	return (ConfigNode*)this->FirstChildElement();
	
}

const char* ConfigFile::getError()
{
	return ErrorName();
}


int ConfigNode::countChildren(const char* name)
{
	int count = 0;
	ConfigNode* p;

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



bool ConfigNode::getConstBoolean(const char* paramName, bool defaultValue)
{
	tinyxml2::XMLElement* pParameter;

	if (!this)
		throw std::exception((std::string("Illegal access to boolean parameter") + std::string(paramName)).c_str());

	if (paramName) pParameter = getChild(paramName);
	else pParameter = this;

	if (pParameter)
	{
		const char* text = pParameter->GetText();
		if (!strcmp(text, "true") || !strcmp(text,"True"))
			return true;
		if (!strcmp(text, "false") || !strcmp(text,"False"))
			return false;
	}

	char msg[128];
	sprintf_s(msg, 128, "Parameter %s/%s not found. Using default value %d", getName(), paramName, defaultValue);
	Logger::logMessage(Warning, msg);

	return defaultValue;
}

int ConfigNode::getConstInteger(const char* paramName, int defaultValue)
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
//	else throw std::exception((std::string("Illegal access to int parameter") + std::string(paramName)).c_str());

	char msg[128];
	sprintf_s(msg, 128, "Parameter %s/%s not found. Using default value %d", getName(), paramName, defaultValue);
	Logger::logMessage(Warning, msg);
	return defaultValue;
}

double ConfigNode::getConstDouble(const char* paramName, double defaultValue)
{
	ConfigNode* pParameter;
	if (!this)
		throw std::exception((std::string("Illegal access to double parameter") + std::string(paramName)).c_str());

	if (paramName) pParameter = getChild(paramName);
	else pParameter = this;

	if (pParameter)
	{
		return atof(pParameter->GetText());
	}
//	else throw std::exception((std::string("Illegal access to double parameter") + std::string(paramName)).c_str());

	char msg[128];
	sprintf_s(msg, 128, "Parameter %s/%s not found. Using default value %f", getName(), paramName, defaultValue);
	Logger::logMessage(Warning, msg);

	return defaultValue;
}

const char* ConfigNode::getConstString(const char* paramName, const char* defaultValue)
{
	ConfigNode* pParameter;
	if (!this)
		throw std::exception((std::string("Illegal access to string parameter") + std::string(paramName)).c_str());

	if (paramName) pParameter = getChild(paramName);
	else pParameter = this;

	if (pParameter)
	{
		return pParameter->GetText();
	}
//	else throw std::exception((std::string("Illegal access to string parameter") + std::string(paramName)).c_str());

	char msg[128];
	sprintf_s(msg, 128, "Parameter %s/%s not found. Using default value %s", getName(), paramName, defaultValue);
	Logger::logMessage(Warning, msg);

	return defaultValue;
}

ConfigNode* ConfigNode::getChild(const char* paramName)
{
	if (!this)
		throw std::exception((std::string("Illegal access to child parameter ") + std::string(paramName)).c_str());

	tinyxml2::XMLElement* child = FirstChildElement(paramName);
	return static_cast<ConfigNode*> (child);
}

ConfigNode* ConfigNode::getNextSibling(const char* paramName)
{
	if (!this)
		throw std::exception((std::string("Illegal access to child parameter ") + std::string(paramName)).c_str());

	tinyxml2::XMLElement* child = NextSiblingElement(paramName);
	return static_cast<ConfigNode*> (child);
}

const char* ConfigNode::getName()
{
	if (!this)
		throw std::exception("Illegal access to inexistant parameter's name");

	return Name();
}

void ConfigNode::saveFile(const char* pFilename)
{
	SaveFile(pFilename, false);
}

void ConfigNode::saveFile(FILE* pFile)
{
	SaveFile(pFile, false);
}

void ConfigNode::clone(ConfigFile* parameterFile)
{
	this->DeleteChildren();

	this->ShallowClone((tinyxml2::XMLDocument*) parameterFile);
}