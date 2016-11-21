#pragma once

#include "app.h"


class RLSimionApp : public CSimionApp
{
public:

	RLSimionApp(CConfigNode* pParameters);
	virtual ~RLSimionApp();

	void getInputFiles(CFilePathList& filePathList);
	void getOutputFiles(CFilePathList& filePathList);
	virtual void setOutputDirectory(const char*);

	void run();
};