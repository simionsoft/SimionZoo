#include "stdafx.h"
#include "app.h"
#include "logger.h"
#include "world.h"
#include "experiment.h"
#include "SimGod.h"
#include "config.h"
#include "utils.h"


CSimionApp* CSimionApp::m_pAppInstance = 0;

CSimionApp::CSimionApp()
{
	m_pAppInstance = this;
}

CSimionApp::~CSimionApp()
{
	m_pAppInstance = 0;
}

const char* CSimionApp::getArgValue(int argc,char** argv,char* argName)
{
	char argPrefix[256];
	sprintf_s(argPrefix, 256, "-%s=", argName);
	for (int i = 1; i < argc; ++i)
	{
		if (strstr(argv[i], argPrefix) == argv[i])
		{
			return (char*)(argv[i]) + strlen(argPrefix);
		}
	}
	return nullptr;
}

bool CSimionApp::flagPassed(int argc, char** argv, char* flagName)
{
	char argPrefix[256];
	sprintf_s(argPrefix, 256, "-%s", flagName);
	for (int i = 1; i < argc; ++i)
	{
		if (!strcmp(argv[i], argPrefix))
		{
			return true;
		}
	}
	return false;
}

void CSimionApp::printInputOutputFiles()
{
	CFilePathList InputFileList, OutputFileList;
	printf("<Files>\n");

	getInputFiles(InputFileList);
	for (int i = 0; i < InputFileList.getNumFilePaths(); i++)
	{
		printf("  <Input>%s</Input>\n", InputFileList.getFilePath(i));
	}

	getOutputFiles(OutputFileList);
	for (int i = 0; i < OutputFileList.getNumFilePaths(); i++)
	{
		printf("  <Output>%s</Output>\n", OutputFileList.getFilePath(i));
	}
	printf("</Files>\n");
}

CSimionApp* CSimionApp::get()
{
	return m_pAppInstance;
}

bool CSimionApp::isExecutedRemotely()
{
#ifdef _DEBUG
	return true;// false;
#else
	return true;
#endif
}
