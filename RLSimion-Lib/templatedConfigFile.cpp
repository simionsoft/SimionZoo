#include "stdafx.h"
#include "templatedConfigFile.h"

#define MAX_CONFIG_FILE_SIZE 10000

CTemplatedConfigFile::CTemplatedConfigFile()
{
	m_pTemplateConfigFileContent = 0;
	m_pAuxBuffer = 0;
	m_pInstantiatedConfigFile = 0;
	m_bTemplateLoaded = false;
}

CTemplatedConfigFile::~CTemplatedConfigFile()
{
	if (m_pTemplateConfigFileContent != 0)
		delete[] m_pTemplateConfigFileContent;
	if (m_pAuxBuffer)
		delete[] m_pAuxBuffer;
	if (m_pInstantiatedConfigFile)
		delete[] m_pInstantiatedConfigFile;
}

bool CTemplatedConfigFile::load(const char* inTemplateConfigFile)
{
	FILE *templateFile;
	int numCharsRead = 0;

	if (m_bTemplateLoaded) return false;


	m_pTemplateConfigFileContent = new char[MAX_CONFIG_FILE_SIZE];
	m_pInstantiatedConfigFile = new char[MAX_CONFIG_FILE_SIZE];
	m_pAuxBuffer = new char[MAX_CONFIG_FILE_SIZE];
	fopen_s(&templateFile, inTemplateConfigFile, "r");
	if (templateFile)
	{
		numCharsRead = fread_s(m_pTemplateConfigFileContent, MAX_CONFIG_FILE_SIZE, sizeof(char)
			, MAX_CONFIG_FILE_SIZE, templateFile);
		m_pTemplateConfigFileContent[numCharsRead] = 0;
		memcpy_s(m_pInstantiatedConfigFile, MAX_CONFIG_FILE_SIZE
			, m_pTemplateConfigFileContent, MAX_CONFIG_FILE_SIZE);

		m_bTemplateLoaded = true;
		return true;
	}
	return false;
}


void CTemplatedConfigFile::instantiate(double value)
{
	sprintf_s(m_pAuxBuffer, MAX_CONFIG_FILE_SIZE, m_pInstantiatedConfigFile, value);

	std::swap(m_pInstantiatedConfigFile, m_pAuxBuffer);
}

void CTemplatedConfigFile::instantiate(int value)
{
	sprintf_s(m_pAuxBuffer, MAX_CONFIG_FILE_SIZE, m_pInstantiatedConfigFile, value);

	std::swap(m_pInstantiatedConfigFile, m_pAuxBuffer);
}

void CTemplatedConfigFile::instantiate(char const* value)
{
	sprintf_s(m_pAuxBuffer, MAX_CONFIG_FILE_SIZE, m_pInstantiatedConfigFile, value);

	std::swap(m_pInstantiatedConfigFile, m_pAuxBuffer);
}

template <typename FirstArgType, typename ...RestArgsType>
void CTemplatedConfigFile::instantiate(FirstArgType firstArg, RestArgsType... restArgs)
{
	instantiate(firstArg);
	instantiate(restArgs...);
}


template <typename FirstArgType, typename ...RestArgsType>
bool CTemplatedConfigFile::instantiateConfigFile(char const* outInstantiatedConfigFile, FirstArgType firstArg
	, RestArgsType... args)
{
	if (m_bTemplateLoaded) return false;

	instantiate(firstArg,args...);

	FILE* outputInstance;
	fopen_s(&outputInstance, outInstantiatedConfigFile, "w");

	if (outputInstance)
	{
		fwrite(m_pInstantiatedConfigFile,1, strlen(m_pInstantiatedConfigFile), outputInstance);
		fclose(outputInstance);
		return true;
	}
	return false;
}
//
//void foo()
//{
//	CTemplatedConfigFile t;
//	t.instantiateConfigFile("a.txt", 2.3);
//}