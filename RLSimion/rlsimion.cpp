// RLSimion-Lib-test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../RLSimion-Lib/app.h"
#include "../RLSimion-Lib/app-rlsimion.h"
#include "../RLSimion-Lib/logger.h"
#include "../RLSimion-Lib/config.h"


int main(int argc, char* argv[])
{

	//_CrtSetBreakAlloc(157);
	try
	{
		CConfigFile configXMLFile;
		CSimionApp* pApp = 0;
		//initialisation required for all apps: create the comm pipe and load the xml configuration file, ....
		if (argc > 2)
			CLogger::createOutputPipe(argv[2]);

		if (argc <= 1)
			CLogger::logMessage(MessageType::Error, "Too few parameters: no config file provided");

		CConfigNode* pParameters= configXMLFile.loadFile(argv[1]);
		if (!pParameters) throw std::exception("Wrong experiment configuration file");

		if (!strcmp("RLSimion", pParameters->getName()))
			pApp = new RLSimionApp(pParameters);
				
		if (pApp)
		{
			pApp->setOutputDirectory(argv[1]);

			pApp->run();

			delete pApp;
		}
		else throw std::exception("Wrong experiment configuration file");
	}
	catch (std::exception& e)
	{
		CLogger::logMessage(MessageType::Error, e.what());
	}
	//_CrtDumpMemoryLeaks();
	return 0;
}