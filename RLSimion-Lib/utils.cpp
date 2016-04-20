#include "stdafx.h"
#include "utils.h"
#include "globals.h"
#include "parameters.h"


CFilePathList::CFilePathList()
{
	m_pBuffer = new char*[m_numMaxFilePaths];
	m_numFilePaths = 0;
}

CFilePathList::~CFilePathList()
{
	for (int i = 0; i < m_numFilePaths; i++)
		delete [] m_pBuffer[i];
	delete[] m_pBuffer;
}

void CFilePathList::addFilePath(const char* filepath)
{
	if (m_numFilePaths < m_numMaxFilePaths - 1)
	{
		m_pBuffer[m_numFilePaths] = new char[m_filePathMaxSize];
		strcpy_s(m_pBuffer[m_numFilePaths], m_filePathMaxSize, filepath);
		m_numFilePaths++;
	}
}

const char* CFilePathList::getFilePath(int i)
{
	if (i < m_numFilePaths) return m_pBuffer[i];

	return 0;
}


CLASS_CONSTRUCTOR(CDirFileOutput)
{
	DIR_PATH_VALUE(m_outputDir, "Output-Dir", "../data", "The output directory");
	CONST_STRING_VALUE(m_filePrefix, "File-Prefix", "", "The prefix given to all the output files");
	END_CLASS();
}

const char* CDirFileOutput::getOutputDir()
{
	return m_outputDir;
}

const char* CDirFileOutput::getFilePrefix()
{
	return m_filePrefix;
}