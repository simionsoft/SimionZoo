#pragma once

#include "app.h"


class RLSimionApp : public CApp
{
public:

	RLSimionApp(int argc,char* argv[]);
	~RLSimionApp();

	int getInputFiles(char* pBuffer[], int& bufferSize);
	int getOutputFiles(char* pBuffer[], int& bufferSize);

	void run();
};