// ControllerToActorCritic.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "../RLSimion-Lib/app-controller-to-vfa.h"
#include "../RLSimion-Lib/logger.h"

int main(int argc, char* argv[])
{
	CApp* pApp = new ControllerToVFAApp(argc, argv);// CApp::getInstance(argc, argv);

	pApp->run();

	delete pApp;

	return 0;
}

