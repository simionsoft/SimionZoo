#pragma once

#include "app.h"


class RLSimionApp : public CApp
{
public:

	RLSimionApp(int argc,char* argv[]);
	~RLSimionApp();

	void getInputFiles(CFilePathList& filePathList);
	void getOutputFiles(CFilePathList& filePathList);

	void run();
};