#include "stdafx.h"
#include "utils.h"
#include "globals.h"
#include "config.h"


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

void CFilePathList::clear()
{
	for (int i = 0; i < m_numFilePaths; i++)
		delete[] m_pBuffer[i];
	m_numFilePaths = 0;
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
