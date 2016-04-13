// RLSimion-Lib-test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "../RLSimion-Lib/app-rlsimion.h"
#include "../RLSimion-Lib/logger.h"

int main(int argc, char* argv[])
{
	try
	{
		CApp* pApp = new RLSimionApp(argc, argv);// CApp::getInstance(argc, argv);

		pApp->run();

		delete pApp;
	}
	catch (std::exception& e)
	{
		CLogger::logMessage(MessageType::Error, e.what());
	}
	
	return 0;
}