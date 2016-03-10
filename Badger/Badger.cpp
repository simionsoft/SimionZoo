// Auto-RLToolbox.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../RLSimion-Lib/parameters.h"

#include "ProcessSpawner.h"

#ifdef _DEBUG
#pragma comment(lib,"../Debug/RLSimion-Lib.lib")
#else
#pragma comment(lib,"../Release/RLSimion-Lib.lib")
#endif


CParameters *g_pCurrentParameters;
int numLeafs = 0;

CProcessSpawner* g_pProcessSpawner;


int g_firstBadgerFileIndex = 0;
int getFirstBadgerFileIndex(CParameters* pParameters)
{
	char fileName[512];
	FILE* pFile;

	if (0 != (int)pParameters->getParameter("RESET_BADGER_INDEX")->getDouble())
		return 0;
	
	int n = -1;
	do
	{
		n++;
		sprintf_s(fileName, 512, "%s/exp-%d.txt", pParameters->getParameter("OUTPUT_DIRECTORY")->getStringPtr(), n);
		fopen_s(&pFile, fileName, "r");
		if (pFile) fclose(pFile);
	} while (pFile != 0);

	return n;
}

void ProcessCommand(CParameters* pAppParameters)
{
	static int numExperiment = 0;
	char fileName[512];
	char commandLine[512];

	//create parameter file for experiment
	sprintf_s(fileName, 512, "%s/exp-%d.txt", pAppParameters->getParameter("OUTPUT_DIRECTORY")->getStringPtr()
		, numExperiment + g_firstBadgerFileIndex);
	g_pCurrentParameters->saveParameters(fileName);

	
	
	//command line
	//using the full name BADGER/EXE_FILE because it comes from the experiment config file, loaded as a table, not a tree
	sprintf_s(commandLine, 512, "%s %s", g_pCurrentParameters->getParameter("BADGER/EXE_FILE")->getStringPtr(), fileName);
	printf("\n\n******************\nBadger\n******************\nExperiment %d: %s\n******************\n\n", numExperiment, commandLine);

	if (0 != (int)pAppParameters->getParameter("RUN_EXPERIMENTS")->getDouble())
	{
		g_pProcessSpawner->spawnOrWait(commandLine);
	}

	if (0 != (int)pAppParameters->getParameter("SAVE_EXPERIMENTS")->getDouble())
	{
		//printf("SAVING COMMAND...");
		FILE* pBatchFile;
		if (numExperiment==0 && 0!= (int)pAppParameters->getParameter("SAVE_EXPERIMENTS_FILE_RESET")->getDouble())
			fopen_s(&pBatchFile, pAppParameters->getParameter("SAVE_EXPERIMENTS_FILE")->getStringPtr(), "w");
		else
			fopen_s(&pBatchFile, pAppParameters->getParameter("SAVE_EXPERIMENTS_FILE")->getStringPtr(),"a");
		if (pBatchFile)
		{
			fprintf_s(pBatchFile, "%s\n", commandLine);
			fclose(pBatchFile);
		}
	}

	//increment experiment-id
	numExperiment++;
}



void TraverseTree(CParameters* pNode,CParameters* pAppParameters)
{
	//pNode is the tree of parameters read from an experiment file (the hierarchy is set using "{" and "}"
	//each parameter name contains its complete path

	//pAppParameters is a regular tree of parameters hanging from the node labeled "BADGER"

	//set parameters, whether is leaf or not
	if (strcmp(pNode->getName(),"Root")!=0)
	{
		for (int i= 0; i<pNode->getNumParameters(); i++)
		{
			//THIS HAS TO BE THE FULLNAME BECAUSE IT COMES FROM THE BADGER EXPERIMENT FILE
			if (!strcmp(pNode->getParameter(i)->getName(), "BADGER/BASE_PARAMETER_FILE"))
			{
				g_pCurrentParameters->reset();
				if (strcmp("NONE", pNode->getParameter(i)->getStringPtr()))
					g_pCurrentParameters->loadParameters(pNode->getParameter(i)->getStringPtr(), false);
			}
			else
				g_pCurrentParameters->addParameter(pNode->getParameter(i));
		}
	}

	if (pNode->getNumChildren()==0)
	{
		numLeafs++;

		ProcessCommand(pAppParameters);

		return;
	}
	else for (int i= 0; i<pNode->getNumChildren(); i++)
	{
		TraverseTree(pNode->getChild(i),pAppParameters);
	}
}


int main(int argc, char* argv[])
{
	CParameters Root;

	CParameters *pCurrent= &Root;
	CParameters *pLastNode= pCurrent;
	CParameter readParameter;
	char line[512];
	FILE *pFile;
	int numParameters= 0;
	int numLines= 0;
	int numExperiments;


	//SetWorkPath();
	CParameters *pAppParameters = new CParameters("../Badger/Badger-parameters.txt");
	CParameters *pBadgerParameters = pAppParameters->getChild("BADGER");

	g_pProcessSpawner = new CProcessSpawner(pBadgerParameters);

	//try to create directory in case it doesn't exist
	_mkdir(pBadgerParameters->getParameter("OUTPUT_DIRECTORY")->getStringPtr());

	g_firstBadgerFileIndex = getFirstBadgerFileIndex(pBadgerParameters);

	g_pCurrentParameters = new CParameters;

	CParameters* pExperimentNode = pBadgerParameters->getChild("EXPERIMENTS");

	numExperiments = pExperimentNode->getNumParameters();

	for (int i = 0; i < numExperiments; i++)
	{
		const char* filename = pExperimentNode->getParameter(i)->getStringPtr();
		fopen_s(&pFile,pExperimentNode->getParameter(i)->getStringPtr(), "r");
		if (pFile)
		{
			//get rid of parameters remaining from previous batch file
			g_pCurrentParameters->reset();
			//go back to the root node
			pCurrent = &Root;
			pLastNode = pCurrent;
			Root.setName("Root");
			Root.reset();

			while (fgets(line, sizeof(line), pFile))
			{
				numLines++;
				if (strstr(line, "{") != 0)
				{
					pLastNode = pCurrent->addChild(0);

					pCurrent = pLastNode;
				}
				else if (strstr(line, "}") != 0)
				{
					pCurrent = pCurrent->getParent();
				}
				else
				{
					if (CParameters::parseLine(line, readParameter))
					{
						pCurrent->addParameter(readParameter);
						numParameters++;	
					}

				}
			}
			fclose(pFile);

			TraverseTree(&Root, pAppParameters->getChild("BADGER"));

			g_pProcessSpawner->waitAll();
		}

	}


	delete g_pProcessSpawner;
	delete pAppParameters;
	delete g_pCurrentParameters;

	return 0;
}

