// RLSimion-Lib-test.cpp : Defines the entry point for the console application.
//

#include "app.h"
#include "app-rlsimion.h"
#include "logger.h"
#include "config.h"


int main(int argc, char* argv[])
{
	try
	{
		CConfigFile configXMLFile;
		CSimionApp* pApp = 0;
		//initialisation required for all apps: create the comm pipe and load the xml configuration file, ....
		const char* pPipename = CSimionApp::getArgValue(argc, argv, "pipe");
		if (pPipename)
		{
			CLogger::m_outputPipe.connectToServer(pPipename);
			//if connection with parent process went ok, we set the logger's output mode accordingly
			if (CLogger::m_outputPipe.isConnected())
				CLogger::m_messageOutputMode = MessageOutputMode::NamedPipe;
		}

		if (argc <= 1)
			CLogger::logMessage(MessageType::Error, "Too few parameters: no config file provided");

		CConfigNode* pParameters= configXMLFile.loadFile(argv[1]);
		if (!pParameters) throw std::exception("Wrong experiment configuration file");

		if (!strcmp("RLSimion", pParameters->getName()) || !strcmp("RLSimion-x64", pParameters->getName()) || !strcmp("RLSimion-x64-CNTK", pParameters->getName()))
			pApp = new RLSimionApp(pParameters);
				
		if (pApp)
		{
			pApp->setConfigFile(argv[1]);

			if (CSimionApp::flagPassed(argc,argv,"requirements"))
				pApp->printRequirements();
			else pApp->run();

			delete pApp;
		}
		else throw std::exception("Wrong experiment configuration file");
	}
	catch (std::exception& e)
	{
		CLogger::logMessage(MessageType::Error, e.what());
	}

	return 0;
}