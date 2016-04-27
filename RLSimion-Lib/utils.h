#pragma once

class CParameters;

class CFilePathList
{
	char** m_pBuffer;
	int m_numFilePaths;
public:
	static const int m_numMaxFilePaths = 1024;
	static const int m_filePathMaxSize = 1024;

	CFilePathList();
	~CFilePathList();

	void clear();

	void addFilePath(const char* filepath);
	int getNumFilePaths() { return m_numFilePaths; }
	const char* getFilePath(int i);
};

class CDirFileOutput
{
	const char* m_outputDir;
	const char* m_filePrefix;
public:
	CDirFileOutput(CParameters* pParameters);
	const char* getOutputDir();
	const char* getFilePrefix();
};