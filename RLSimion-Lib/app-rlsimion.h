#pragma once

#include "app.h"
#define MAX_PATH_SIZE 1024

class RLSimionApp : public CSimionApp
{
	char m_directory[MAX_PATH_SIZE];
public:

	RLSimionApp(CConfigNode* pParameters);
	virtual ~RLSimionApp();

	void getInputFiles(CFilePathList& filePathList);
	void getOutputFiles(CFilePathList& filePathList);
	void setOutputDirectory(const char*);
	char* getOutputDirectory();

	void run();
};