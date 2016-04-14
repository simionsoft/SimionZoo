#include "stdafx.h"
#include "../RLSimion-Lib/app-compare-controller-vfa.h"
#include "../RLSimion-Lib/logger.h"

int main(int argc, char* argv[])
{
	try
	{
		CApp* pApp = new CompareControllerVFAApp(argc, argv);// CApp::getInstance(argc, argv);

		pApp->run();

		delete pApp;
	}
	catch (std::exception& e)
	{
		CLogger::logMessage(MessageType::Error, e.what());
	}
	return 0;
}

