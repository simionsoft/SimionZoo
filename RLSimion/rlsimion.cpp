// RLSimion-Lib-test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "../RLSimion-Lib/app-rlsimion.h"
#include "../RLSimion-Lib/logger.h"

int main(int argc, char* argv[])
{
	CApp app;
	CApp* pApp = CApp::getInstance(argc,argv);

	pApp->init(argc, argv);

	pApp->run();

	delete pApp;
	
	return 0;
}