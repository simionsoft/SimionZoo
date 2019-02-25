#include "run-time-requirements.h"

#include <string.h>

#include "../../tools/System/CrossPlatform.h"


RunTimeRequirements::RunTimeRequirements()
{
}

RunTimeRequirements::~RunTimeRequirements()
{
	for (auto it = m_inputFiles.begin(); it != m_inputFiles.end(); it++)
	{
		if (*it != 0)
			delete *it;
	}
	m_inputFiles.clear();

	for (auto it = m_inputFilesRenamed.begin(); it != m_inputFilesRenamed.end(); it++)
	{
		if (*it != 0)
			delete *it;
	}
	m_inputFilesRenamed.clear();

	for (auto it = m_outputFiles.begin(); it != m_outputFiles.end(); it++) delete (*it);
	m_outputFiles.clear();
}

void RunTimeRequirements::addInputFile(const char* inputFile, const char* inputFileRenamed)
{
	char* copy = new char[strlen(inputFile) + 1];
	CrossPlatform::Strcpy_s(copy, strlen(inputFile) + 1, inputFile);
	m_inputFiles.push_back(copy);
	if (inputFileRenamed != 0)
	{
		copy = new char[strlen(inputFileRenamed) + 1];
		CrossPlatform::Strcpy_s(copy, strlen(inputFileRenamed) + 1, inputFileRenamed);
		m_inputFilesRenamed.push_back(copy);
	}
	else m_inputFilesRenamed.push_back(0);
}

unsigned int RunTimeRequirements::getNumInputFiles()
{
	return (unsigned int)m_inputFiles.size();
}

const char* RunTimeRequirements::getInputFile(unsigned int i)
{
	if (i < m_inputFiles.size())
		return m_inputFiles[i];
	return 0;
}

const char* RunTimeRequirements::getInputFileRename(unsigned int i)
{
	if (i < m_inputFilesRenamed.size())
		return m_inputFilesRenamed[i];
	return 0;
}

unsigned int RunTimeRequirements::getNumOutputFiles()
{
	return (unsigned int)m_outputFiles.size();
}

const char* RunTimeRequirements::getOutputFile(unsigned int i)
{
	if (i < m_outputFiles.size())
		return m_outputFiles[i];
	return 0;
}


void RunTimeRequirements::addOutputFile(const char* filepath)
{
	char* copy = new char[strlen(filepath) + 1];
	CrossPlatform::Strcpy_s(copy, strlen(filepath) + 1, filepath);
	m_outputFiles.push_back(copy);
}

#define INPUT_FILE_XML_TAG "Input-File"
#define OUTPUT_FILE_XML_TAG "Output-File"
#define RENAME_XML_ATTR "Rename"

void RunTimeRequirements::printXML()
{
	const char* pFileName;
	const char* pFileRename;
	for (unsigned int i = 0; i < getNumInputFiles(); i++)
	{
		pFileName = getInputFile(i);
		pFileRename = getInputFileRename(i);
		if (pFileRename == 0)
			printf("  <%s>%s</%s>\n", INPUT_FILE_XML_TAG, pFileName, INPUT_FILE_XML_TAG);
		else
			printf("  <%s %s=\"%s\">%s</%s>\n", INPUT_FILE_XML_TAG, RENAME_XML_ATTR, pFileRename, pFileName, INPUT_FILE_XML_TAG);
	}

	for (unsigned int i = 0; i < getNumOutputFiles(); i++)
	{
		pFileName = getOutputFile(i);
		printf("  <%s>%s</%s>\n", OUTPUT_FILE_XML_TAG, pFileName, OUTPUT_FILE_XML_TAG);
	}
}