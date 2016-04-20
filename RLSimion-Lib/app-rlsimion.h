#pragma once

#include "app.h"


class RLSimionApp : public CApp
{
public:

	RLSimionApp(int argc,char* argv[]);
	~RLSimionApp();

//	int getInputFiles(char* pBuffer[], int& numItems);
//	int getOutputFiles(char* pBuffer[], int& numItems);

	void run();
};