// RLSimion-Lib-test.cpp : Defines the entry point for the console application.
//

#include "app.h"
#include "logger.h"
#include "config.h"


int main(int argc, char* argv[])
{
	try
	{
		ConfigFile configXMLFile;
		SimionApp* pApp = 0;
		//initialisation required for all apps: create the comm pipe and load the xml configuration file, ....
		const char* pPipename = SimionApp::getArgValue(argc, argv, "pipe");
		if (pPipename)
		{
			Logger::m_outputPipe.connectToServer(pPipename);
			//if connection with parent process went ok, we set the logger's output mode accordingly
			if (Logger::m_outputPipe.isConnected())
				Logger::m_messageOutputMode = MessageOutputMode::NamedPipe;
		}

		if (argc <= 1)
			Logger::logMessage(MessageType::Error, "Too few parameters: no config file provided");

		ConfigNode* pParameters= configXMLFile.loadFile(argv[1]);
		if (!pParameters) throw std::exception("Wrong experiment configuration file");

		if (SimionApp::flagPassed(argc, argv, "requirements"))
			Logger::enableLogMessages(false);

		if (!strcmp("RLSimion", pParameters->getName()) || !strcmp("RLSimion-x64", pParameters->getName()) || !strcmp("RLSimion-x64-CNTK", pParameters->getName()))
			pApp = new SimionApp(pParameters);
				
		if (pApp)
		{
			pApp->setConfigFile(argv[1]);

			if (SimionApp::flagPassed(argc, argv, "local"))
				pApp->setExecutedRemotely(false);

			if (SimionApp::flagPassed(argc, argv, "requirements"))
				pApp->printRequirements();
			else pApp->run();

			delete pApp;
		}
		else throw std::exception("Wrong experiment configuration file");
	}
	catch (std::exception& e)
	{
		Logger::logMessage(MessageType::Error, e.what());
	}

	return 0;
}