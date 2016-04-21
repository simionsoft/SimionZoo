// TESTInputOutputFile.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../../RLSimion-Lib/app-rlsimion.h"
#include "../../RLSimion-Lib/app-controller-to-vfa.h"
#include "../../RLSimion-Lib/app-compare-controller-vfa.h"
#include "../../RLSimion-Lib/logger.h"
#include "../../RLSimion-Lib/parameters.h"
#include "../../RLSimion-Lib/utils.h"


void main(int argc, char* argv[])
{
	CParameterFile configXMLFile;
	CApp* pApp = 0;
	CFilePathList InputFileList;
	CFilePathList OutputFileList;
	char buffer[10000];
	char* pBuffer = buffer;
	char* pBufferEnd = pBuffer + 10000;
	try
	{
		if (argc <= 1)
			CLogger::logMessage(MessageType::Error, "Too few parameters");

		CParameters* pParameters = configXMLFile.loadFile(argv[1]);
		if (!pParameters) throw std::exception("Wrong experiment configuration file");

		if (!strcmp("RLSimion", pParameters->getName()))
			pApp = new RLSimionApp(pParameters, argv[1]);
		else if (!strcmp("ControllerToVFA", pParameters->getName()))
			pApp = new ControllerToVFAApp(pParameters);
		else if (!strcmp("CompareControllerVFA", pParameters->getName()))
			pApp = new CompareControllerVFAApp(pParameters);

		if (pApp)
		{
			strcpy_s(pBuffer, pBufferEnd - pBuffer, "<Files>\n");
			pBuffer += strlen("<Files>\n") ;
			pApp->getInputFiles(InputFileList);
			for (int i = 0; i < InputFileList.getNumFilePaths(); i++)
			{
				strcpy_s(pBuffer, pBufferEnd - pBuffer, "<Input>");
				pBuffer += strlen("<Input>") ;
				strcpy_s(pBuffer, pBufferEnd - pBuffer, InputFileList.getFilePath(i));
				pBuffer += strlen(InputFileList.getFilePath(i)) ;
				strcpy_s(pBuffer, pBufferEnd - pBuffer, "</Input>\n");
				pBuffer += strlen("</Input>\n") ;
			}

			pApp->getOutputFiles(OutputFileList);
			for (int i = 0; i < OutputFileList.getNumFilePaths(); i++)
			{
				strcpy_s(pBuffer, pBufferEnd - pBuffer, "<Output>");
				pBuffer += strlen("<Output>") ;
				strcpy_s(pBuffer, pBufferEnd - pBuffer, OutputFileList.getFilePath(i));
				pBuffer += strlen(OutputFileList.getFilePath(i)) ;
				strcpy_s(pBuffer, pBufferEnd - pBuffer, "</Output>\n");
				pBuffer += strlen("</Output>\n") ;
			}
			strcpy_s(pBuffer, pBufferEnd - pBuffer, "</Files>");
			pBuffer += strlen("</Files>") + 1;

			printf("%s", buffer);

			delete pApp;
		}
		else throw std::exception("Wrong experiment configuration file");
	}
	catch (std::exception& e)
	{
		CLogger::logMessage(MessageType::Error, e.what());
	}
}

