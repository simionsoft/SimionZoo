#pragma once

class CConfigNode;

class CFilePathList
{
	char** m_pBuffer;
	int m_numFilePaths;
public:
	static const int m_numMaxFilePaths = 1024;
	static const int m_filePathMaxSize = 1024;

	CFilePathList();
	virtual ~CFilePathList();

	void clear();

	void addFilePath(const char* filepath);
	int getNumFilePaths() { return m_numFilePaths; }
	const char* getFilePath(int i);
};
