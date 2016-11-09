#pragma once

#include "app.h"


class RLSimionApp : public CApp
{
public:

	RLSimionApp(CConfigNode* pParameters,const char* xmlConfigPath);
	virtual ~RLSimionApp();

	void getInputFiles(CFilePathList& filePathList);
	void getOutputFiles(CFilePathList& filePathList);

	void run();
};