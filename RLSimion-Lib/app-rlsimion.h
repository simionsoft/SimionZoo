#pragma once

#include "app.h"


class RLSimionApp : public CApp
{
public:

	RLSimionApp(int argc,char* argv[]);
	~RLSimionApp();

	void getInputs(const char* xmlConfigFile);
	void getOutputs(const char* xmlConfigFile);

	void run();
};