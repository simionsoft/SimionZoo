#pragma once

#include "app.h"


class RLSimionApp : public CApp
{
public:

	RLSimionApp(CParameters* pParameters,int argc,char* argv[]);
	~RLSimionApp();

	void run();
};