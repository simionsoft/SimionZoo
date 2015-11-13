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

#define MAX_NUM_CHILDREN 16

#define MAX_NUM_PARAM_PER_NODE 16
#define PARAMETER_NAME_MAX_SIZE 512
struct Node
{
	int numParameters;
	CParameter parameters[MAX_NUM_PARAM_PER_NODE];


	int numChildren;
	Node *pChildren[MAX_NUM_CHILDREN];
	Node *pParent;
	Node()
	{
		numParameters= 0;
		numChildren= 0;
		pParent= 0;
		//name[0]= 0;
		//value= -1.0;
	}
	~Node()
	{
		for (int i= 0; i<numChildren; i++) delete pChildren[i];
	}
};

CParameters *g_pCurrentParameters;
int numLeafs = 0;

CProcessSpawner* g_pProcessSpawner;


int g_firstBadgerFileIndex = 0;
int getFirstBadgerFileIndex(CParameters* pParameters)
{
	char fileName[512];
	FILE* pFile;

	if (0 != (int)pParameters->getDouble("BADGER/RESET_BADGER_INDEX"))
		return 0;
	
	int n = -1;
	do
	{
		n++;
		sprintf_s(fileName, 512, "%s\\exp-%d.txt", pParameters->getStringPtr("BADGER/OUTPUT_DIRECTORY"), n);
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
	sprintf_s(fileName, 512, "%s\\exp-%d.txt", pAppParameters->getStringPtr("BADGER/OUTPUT_DIRECTORY"), numExperiment + g_firstBadgerFileIndex);
	g_pCurrentParameters->saveParameters(fileName);

	
	
	//command line
	sprintf_s(commandLine, 512, "%s %s", pAppParameters->getStringPtr("BADGER/EXE_FILE"), fileName);
	printf("\n\n******************\nBadger\n******************\nExperiment %d: %s\n******************\n\n", numExperiment, commandLine);

	if (0 != (int)pAppParameters->getDouble("BADGER/RUN_EXPERIMENTS"))
	{
//		printf("RUNNING...\n\n\n");
		g_pProcessSpawner->spawnOrWait(commandLine);
		//system(commandLine);
//		printf("DONE\n\n\n");
	}

	if (0 != (int)pAppParameters->getDouble("BADGER/SAVE_EXPERIMENTS"))
	{
		//printf("SAVING COMMAND...");
		FILE* pBatchFile;
		if (numExperiment==0 && 0!= (int)pAppParameters->getDouble("BADGER/SAVE_EXPERIMENTS_FILE_RESET"))
			fopen_s(&pBatchFile, pAppParameters->getStringPtr("BADGER/SAVE_EXPERIMENTS_FILE"), "w");
		else
			fopen_s(&pBatchFile, pAppParameters->getStringPtr("BADGER/SAVE_EXPERIMENTS_FILE"),"a");
		if (pBatchFile)
		{
			fprintf_s(pBatchFile, "%s\n", commandLine);
			fclose(pBatchFile);
		//	printf("DONE\n");
		}
		//else
		//	printf("FAILED\n");
	}

	//increment experiment-id
	numExperiment++;
}



void TraverseTree(Node* pNode,CParameters* pAppParameters)
{


	//set parameters, whether is leaf or not
	if (strcmp(pNode->parameters[0].name,"Root")!=0)
	{
		for (int i= 0; i<pNode->numParameters; i++)
		{
			if (!strcmp(pNode->parameters[i].name, "BADGER/BASE_PARAMETER_FILE"))
				g_pCurrentParameters->loadParameters((char*)pNode->parameters[i].pValue);
			else
				g_pCurrentParameters->setParameter(pNode->parameters[i]);
		}
	}

	if (pNode->numChildren==0)
	{
		numLeafs++;

		ProcessCommand(pAppParameters);

		return;
	}
	else for (int i= 0; i<pNode->numChildren; i++)
	{
		TraverseTree(pNode->pChildren[i],pAppParameters);
	}
}


int main(int argc, char* argv[])
{
	Node Root;

	strcpy_s(Root.parameters[0].name, 512, "Root");

	Node *pCurrent= &Root;
	Node *pLastNode= pCurrent;
	char line[512];
	char experimentFile[1024];
	FILE *pFile;
	int numParameters= 0;
	int numLines= 0;
	int numExperiments;


	//SetWorkPath();
	CParameters *pAppParameters = new CParameters("Badger-parameters.txt");

	g_pProcessSpawner = new CProcessSpawner((int)pAppParameters->getDouble("BADGER/MAX_NUM_PROCESSES"));

	//try to create directory in case it doesn't exist
	_mkdir(pAppParameters->getStringPtr("BADGER/OUTPUT_DIRECTORY"));

	g_firstBadgerFileIndex = getFirstBadgerFileIndex(pAppParameters);

	g_pCurrentParameters = new CParameters(pAppParameters->getStringPtr("BADGER/BASE_PARAMETER_FILE"));

	numExperiments = (int)pAppParameters->getDouble("BADGER/NUM_EXPERIMENTS");

	for (int i = 0; i < numExperiments; i++)
	{
		sprintf_s(experimentFile, 1024, "BADGER/EXPERIMENT_%d", i);
		fopen_s(&pFile,pAppParameters->getStringPtr(experimentFile), "r");
		if (pFile)
		{
			while (fgets(line, sizeof(line), pFile))
			{
				numLines++;
				if (strstr(line, "{") != 0)
				{
					pLastNode = new Node;
					pCurrent->pChildren[pCurrent->numChildren] = pLastNode;
					pCurrent->numChildren++;

					pLastNode->pParent = pCurrent;
					pCurrent = pLastNode;

					//ReadNodeParameters(line,pLastNode);

				}
				else if (strstr(line, "}") != 0)
				{
					pCurrent = pCurrent->pParent;
				}
				else //if (sscanf_s(line, "%s : %lf", name,&value)==2)
				{
					CParameters::parseLine(line, pCurrent->parameters[pCurrent->numParameters]);
					pCurrent->numParameters++;

					numParameters++;
				}
				//else printf("ERROR in line %d\n",numLines);

			}
			fclose(pFile);
			//printf("%d lines and %d parameters read\n", numLines, numParameters);
		}

	}
	TraverseTree(&Root, pAppParameters);

	g_pProcessSpawner->waitAll();

	delete g_pProcessSpawner;
	delete pAppParameters;
	delete g_pCurrentParameters;

	return 0;
}

