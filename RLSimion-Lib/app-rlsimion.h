#pragma once

#include "app.h"


class RLSimionApp : public CApp
{
public:

	RLSimionApp(CParameters* pParameters,const char* xmlConfigPath);
	virtual ~RLSimionApp();

	void getInputFiles(CFilePathList& filePathList);
	void getOutputFiles(CFilePathList& filePathList);

	void run();
};