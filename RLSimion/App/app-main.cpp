/*
	SimionZoo: A framework for online model-free Reinforcement Learning on continuous
	control problems

	Copyright (c) 2016 SimionSoft. https://github.com/simionsoft

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

// RLSimion-Lib-test.cpp : Defines the entry point for the console application.
//

#include "../Lib/app.h"
#include "../Lib/logger.h"
#include "../Lib/config.h"
#include "../../tools/System/FileUtils.h"

int main(int argc, char* argv[])
{
	try
	{
		//set the executable's directory as the current directory. This is required under Linux to be able to pass a relative path
		string dir= getDirectory(string(argv[0]));
		changeWorkingDirectory(dir);

		ConfigFile configXMLFile;
		SimionApp* pApp = 0;
		//initialisation required for all apps: create the comm pipe and load the xml configuration file, ....
		const char* pPipename = SimionApp::getArgValue(argc, argv, "pipe");
		if (pPipename)
		{
			Logger::m_outputPipe.connectToServer(pPipename, true); //prefix must added under Windows (.////pipe//) and under Linux (/tmp/)

			//if connection with parent process went ok, we set the logger's output mode accordingly
			if (Logger::m_outputPipe.isConnected())
			{
				Logger::m_messageOutputMode = MessageOutputMode::NamedPipe;
				Logger::logMessage(MessageType::Info, "Succesfully connected to output named pipe");
			}
			else
				Logger::logMessage(MessageType::Info, "Failed to connect to output named pipe");
		}

		if (argc <= 1)
			Logger::logMessage(MessageType::Error, "Too few parameters: no config file provided");

		ConfigNode* pParameters= configXMLFile.loadFile(argv[1]);
		if (!pParameters) throw std::runtime_error("Wrong experiment configuration file");

		if (SimionApp::flagPassed(argc, argv, "requirements"))
			Logger::enableLogMessages(false);

		if (!strcmp("RLSimion", pParameters->getName()) || !strcmp("RLSimion-x64", pParameters->getName()) )
			pApp = new SimionApp(pParameters);

		if (pApp)
		{
			pApp->setConfigFile(argv[1]);

			//if running locally, we show the graphical window
			if (SimionApp::flagPassed(argc, argv, "local"))
				pApp->setExecutedRemotely(false);
			else pApp->setExecutedRemotely(true);

			//CPU is used by default.
			//tests so far seem to run faster on multi-core cpus than using gpus O_o
			if (SimionApp::flagPassed(argc, argv, "gpu"))
				pApp->setPreferredDevice(Device::GPU);
			else pApp->setPreferredDevice(Device::CPU);

			if (SimionApp::flagPassed(argc, argv, "requirements"))
				pApp->printRequirements();
			else pApp->run();

			delete pApp;
		}
		else throw std::runtime_error("Wrong experiment configuration file");
	}
	catch (std::exception& e)
	{
		Logger::logMessage(MessageType::Error, e.what());
	}

	return 0;
}