// TESTInputOutputFile.cpp : Defines the entry point for the console application.
//

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#include <crtdbg.h>
#define _CRTDBG_MAP_ALLOC
#endif  // _DEBUG

#include <stdlib.h>
#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#ifdef _DEBUG
#pragma comment (lib,"../../Debug/RLSimion-Lib.lib")
#pragma comment (lib,"../../Debug/tinyxml2.lib")
#else
#pragma comment (lib,"../../Release/RLSimion-Lib.lib")
#pragma comment (lib,"../../Release/tinyxml2.lib")
#endif

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

