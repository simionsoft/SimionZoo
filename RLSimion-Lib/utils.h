#pragma once

class CParameters;

class CDirFileOutput
{
	const char* m_outputDir;
	const char* m_filePrefix;
public:
	CDirFileOutput(CParameters* pParameters);
	const char* getOutputDir();
	const char* getFilePrefix();
};