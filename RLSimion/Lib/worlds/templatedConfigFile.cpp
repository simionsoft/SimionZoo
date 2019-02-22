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

#include "templatedConfigFile.h"


TemplatedConfigFile::TemplatedConfigFile()
{
	m_pTemplateConfigFileContent = 0;
	m_pInstantiatedConfigFile = 0;
	m_bTemplateLoaded = false;
}

TemplatedConfigFile::~TemplatedConfigFile()
{
	if (m_pTemplateConfigFileContent != 0)
		delete[] m_pTemplateConfigFileContent;
	if (m_pInstantiatedConfigFile)
		delete[] m_pInstantiatedConfigFile;
}

bool TemplatedConfigFile::load(const char* inTemplateConfigFile)
{
	FILE *templateFile;
	int numCharsRead = 0;

	if (m_bTemplateLoaded) return true;

	m_pTemplateConfigFileContent = new char[MAX_CONFIG_FILE_SIZE];
	m_pInstantiatedConfigFile = new char[MAX_CONFIG_FILE_SIZE];
	CrossPlatform::Fopen_s(&templateFile, inTemplateConfigFile, "r");
	if (templateFile)
	{
		numCharsRead = CrossPlatform::Fread_s(m_pTemplateConfigFileContent, MAX_CONFIG_FILE_SIZE, sizeof(char)
			, MAX_CONFIG_FILE_SIZE, templateFile);
		m_pTemplateConfigFileContent[numCharsRead] = 0;

		m_bTemplateLoaded = true;
		return true;
	}
	return false;
}



//for some unknown reason, VStudio fails to link to the versions of the templated functions that are only called from an implementation
//of a virtual function, so this little function foo() with the signatures we use seems like a small price for getSample to work
void foo()
{
	TemplatedConfigFile t;
	t.instantiateConfigFile("a.txt", 32.3);
	t.instantiateConfigFile("a.txt", 2.3,3.4,13.3);
	t.instantiateConfigFile("a.txt", 2.3, 5.2, "ajal");
	t.instantiateConfigFile("a.txt", "b");
}