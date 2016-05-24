// TESTInputOutputFile.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "../../RLSimion-Lib/logger.h"

#include "../RLSimionInterfaceDLL/RLSimionInterfaceDLL.h"
#pragma comment(lib,"../../Debug/RLSimionInterfaceDLL.lib")

void main(int argc, char* argv[])
{

	char buffer[100000];
	//_CrtSetBreakAlloc(405);

	try
	{
		if (argc <= 1)
			CLogger::logMessage(MessageType::Error, "Too few parameters");

		for (int fileIndex = 1; fileIndex < argc; fileIndex++)
		{

				getIOFiles(argv[fileIndex], buffer, 100000);

				printf("%s==========\n%s\n\n",argv[fileIndex], buffer);
		}
	}
	catch (std::exception& e)
	{
		CLogger::logMessage(MessageType::Error, e.what());
	}
	_CrtDumpMemoryLeaks();
}

