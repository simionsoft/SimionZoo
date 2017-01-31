// binLogFileReadTest.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <stdio.h>
#include <memory>
#include "../../RLSimion-Lib/logger.h"
#include <stdio.h>
#include <tchar.h>

#define HEADER_MAX_SIZE 16
#define EXPERIMENT_HEADER 1
#define EPISODE_HEADER 2
#define STEP_HEADER 3
#define EPISODE_END_HEADER 4

struct ExperimentHeader
{
	__int64 magicNumber = EXPERIMENT_HEADER;
	__int64 fileVersion = CLogger::BIN_FILE_VERSION;
	__int64 numEpisodes = 0;

	__int64 padding[HEADER_MAX_SIZE - 3]; //extra space
	ExperimentHeader()
	{
		memset(padding, 0, sizeof(padding));
	}
};

struct EpisodeHeader
{
	__int64 magicNumber = EPISODE_HEADER;
	__int64 episodeType;
	__int64 episodeIndex;
	__int64 numVariablesLogged;

	__int64 padding[HEADER_MAX_SIZE - 4]; //extra space
	EpisodeHeader()
	{
		memset(padding, 0, sizeof(padding));
	}
};

struct StepHeader
{
	__int64 magicNumber = STEP_HEADER;
	__int64 stepIndex;

	double experimentRealTime;
	double episodeSimTime;
	double episodeRealTime;

	__int64 padding[HEADER_MAX_SIZE - 5]; //extra space
	StepHeader()
	{
		memset(padding, 0, sizeof(padding));
	}
};

int _tmain(int argc, _TCHAR* argv[])
{
	ExperimentHeader experimentHeader;
	EpisodeHeader episodeHeader;
	StepHeader stepHeader;

	FILE * pFile;
	double loggedVariables[1000];
	fopen_s(&pFile, "experiment-log.bin", "rb");
	int a = sizeof(ExperimentHeader);
	int b = sizeof(EpisodeHeader);
	int c = sizeof(StepHeader);

	
	if (pFile)
	{
		fread_s((void*)&experimentHeader, sizeof(ExperimentHeader), sizeof(ExperimentHeader), 1, pFile);
		for (int i = 0; i < experimentHeader.numEpisodes; i++)
		{
			fread_s((void*)&episodeHeader, sizeof(EpisodeHeader), sizeof(EpisodeHeader), 1, pFile);

			fread_s((void*)&stepHeader, sizeof(StepHeader), sizeof(StepHeader), 1, pFile);

			while (stepHeader.magicNumber != EPISODE_END_HEADER)
			{
				fread_s((void*)loggedVariables, sizeof(double)*1000, sizeof(double), (size_t)episodeHeader.numVariablesLogged, pFile);
				fread_s((void*)&stepHeader, sizeof(StepHeader), sizeof(StepHeader), 1, pFile);
			}
		}
		fclose(pFile);
	}
	return 0;
}

