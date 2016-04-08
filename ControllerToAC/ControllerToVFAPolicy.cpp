// ControllerToActorCritic.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "../RLSimion-Lib/app-controller-to-vfa.h"
#include "../RLSimion-Lib/logger.h"

int main(int argc, char* argv[])
{
	try
	{
		ControllerToVFAApp app;
		
		app.init(argc, argv);

		app.run();
	}
	catch (std::exception& e)
	{
		CLogger::logMessage(MessageType::Error, e.what());
	}

}

