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

#include "config.h"
#include "experiment.h"
#include "logger.h"
#include "../../tools/System/CrossPlatform.h"

/// <summary>
/// Loads a configuration file and returns its root node. If not found throws an exception
/// </summary>
/// <param name="fileName">The path to the configuration file</param>
/// <param name="nodeName">The name of the root node</param>
/// <returns>A pointer to the configuration root node. nullptr if the name given wasn't found</returns>
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

/// <summary>
/// Returns the number of children this node has with the given name
/// </summary>
int ConfigNode::countChildren(const char* name)
{
	int count = 0;
	ConfigNode* p;
	
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

/// <summary>
/// Retrieves the value of a parameter as a boolean
/// </summary>
/// <param name="paramName">The name of the parameter</param>
/// <param name="defaultValue">Its default value (will be used if the parameter is not found)</param>
/// <returns>The parameter's value</returns>
bool ConfigNode::getConstBoolean(const char* paramName, bool defaultValue)
{
	tinyxml2::XMLElement* pParameter;

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
	CrossPlatform::Sprintf_s(msg, 128, "Parameter %s/%s not found. Using default value %d", getName(), paramName, defaultValue);
	Logger::logMessage(Warning, msg);

	return defaultValue;
}

/// <summary>
/// Retrieves the value of a parameter as an integer
/// </summary>
/// <param name="paramName">The name of the parameter</param>
/// <param name="defaultValue">Its default value (will be used if the parameter is not found)</param>
/// <returns>The parameter's value</returns>
int ConfigNode::getConstInteger(const char* paramName, int defaultValue)
{
	tinyxml2::XMLElement* pParameter;

	if (paramName) pParameter = getChild(paramName);
	else pParameter = this;

	if (pParameter)
	{
		return atoi(pParameter->GetText());
	}
//	else throw std::runtime_error((std::string("Illegal access to int parameter") + std::string(paramName)).c_str());

	char msg[128];
	CrossPlatform::Sprintf_s(msg, 128, "Parameter %s/%s not found. Using default value %d", getName(), paramName, defaultValue);
	Logger::logMessage(Warning, msg);
	return defaultValue;
}

/// <summary>
/// Retrieves the value of a parameter as a double
/// </summary>
/// <param name="paramName">The name of the parameter</param>
/// <param name="defaultValue">Its default value (will be used if the parameter is not found)</param>
/// <returns>The parameter's value</returns>
double ConfigNode::getConstDouble(const char* paramName, double defaultValue)
{
	ConfigNode* pParameter;

	if (paramName) pParameter = getChild(paramName);
	else pParameter = this;

	if (pParameter)
	{
		return atof(pParameter->GetText());
	}
//	else throw std::runtime_error((std::string("Illegal access to double parameter") + std::string(paramName)).c_str());

	char msg[128];
	CrossPlatform::Sprintf_s(msg, 128, "Parameter %s/%s not found. Using default value %f", getName(), paramName, defaultValue);
	Logger::logMessage(Warning, msg);

	return defaultValue;
}

/// <summary>
/// Retrieves the value of a parameter as a C-style string
/// </summary>
/// <param name="paramName">The name of the parameter</param>
/// <param name="defaultValue">Its default value (will be used if the parameter is not found)</param>
/// <returns>The parameter's value</returns>
const char* ConfigNode::getConstString(const char* paramName, const char* defaultValue)
{
	ConfigNode* pParameter;

	if (paramName) pParameter = getChild(paramName);
	else pParameter = this;

	if (pParameter)
	{
		return pParameter->GetText();
	}
//	else throw std::runtime_error((std::string("Illegal access to string parameter") + std::string(paramName)).c_str());

	char msg[128];
	CrossPlatform::Sprintf_s(msg, 128, "Parameter %s/%s not found. Using default value %s", getName(), paramName, defaultValue);
	Logger::logMessage(Warning, msg);

	return defaultValue;
}

/// <summary>
/// Returns the first child with the given name
/// </summary>
/// <param name="paramName">The child's name</param>
/// <returns>A pointer to the child ConfigNode. Might be nullptr if not found</returns>
ConfigNode* ConfigNode::getChild(const char* paramName)
{
	tinyxml2::XMLElement* child = FirstChildElement(paramName);
	return static_cast<ConfigNode*> (child);
}

/// <summary>
/// Returns the next parameter on the level of the current node with the given name
/// </summary>
/// <param name="paramName">The parameter's name</param>
/// <returns>A pointer to the parameter node. Might be nullptr if not found</returns>
ConfigNode* ConfigNode::getNextSibling(const char* paramName)
{
	tinyxml2::XMLElement* child = NextSiblingElement(paramName);
	return static_cast<ConfigNode*> (child);
}

/// <summary>
/// Returns the name of this ConfigNode
/// </summary>
const char* ConfigNode::getName()
{
	return Name();
}

/// <summary>
/// Saves all the configuration nodes below the current to a file
/// </summary>
/// <param name="pFilename">The path to the file</param>
void ConfigNode::saveFile(const char* pFilename)
{
	SaveFile(pFilename, false);
}

/// <summary>
/// Saves all the configuration nodes below the current to an already open file
/// </summary>
/// <param name="pFile">The handle to the already open file</param>
void ConfigNode::saveFile(FILE* pFile)
{
	SaveFile(pFile, false);
}


/// <summary>
/// Makes a shallow copy of the a configuration file
/// </summary>
/// <param name="parameterFile"></param>
void ConfigNode::clone(ConfigFile* parameterFile)
{
	this->DeleteChildren();

	this->ShallowClone((tinyxml2::XMLDocument*) parameterFile);
}