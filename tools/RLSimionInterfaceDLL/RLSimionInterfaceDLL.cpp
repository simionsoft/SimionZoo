// RLSimionInterfaceDLL.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "RLSimionInterfaceDLL.h"
#include "../../RLSimion-Lib/app-rlsimion.h"
#include "../../RLSimion-Lib/app-controller-to-vfa.h"
#include "../../RLSimion-Lib/app-compare-controller-vfa.h"
#include "../../RLSimion-Lib/logger.h"
#include "../../RLSimion-Lib/parameters.h"
#include "../../RLSimion-Lib/utils.h"

// This is an example of an exported variable
RLSIMIONINTERFACEDLL_API int nRLSimionInterfaceDLL=0;

// This is an example of an exported function.
extern "C" __declspec(dllexport) int getIOFiles(const char* xmlFilename, char* pBuffer, int bufferSize)
{
	CParameterFile configXMLFile;
	CApp* pApp = 0;
	CFilePathList InputFileList;
	CFilePathList OutputFileList;
	char* pBufferEnd = pBuffer + bufferSize;
	try
	{
		if (!xmlFilename || !pBuffer) return -1;

		CParameters* pParameters = configXMLFile.loadFile(xmlFilename);
		if (!pParameters) throw std::exception("Wrong experiment configuration file");

		if (!strcmp("RLSimion", pParameters->getName()))
			pApp = new RLSimionApp(pParameters, xmlFilename);
		else if (!strcmp("ControllerToVFA", pParameters->getName()))
			pApp = new ControllerToVFAApp(pParameters);
		else if (!strcmp("CompareControllerVFA", pParameters->getName()))
			pApp = new CompareControllerVFAApp(pParameters);

		if (pApp)
		{
			strcpy_s(pBuffer, pBufferEnd - pBuffer, "<Files>\n");
			pBuffer += strlen("<Files>\n");
			pApp->getInputFiles(InputFileList);
			for (int i = 0; i < InputFileList.getNumFilePaths(); i++)
			{
				strcpy_s(pBuffer, pBufferEnd - pBuffer, "<Input>");
				pBuffer += strlen("<Input>");
				strcpy_s(pBuffer, pBufferEnd - pBuffer, InputFileList.getFilePath(i));
				pBuffer += strlen(InputFileList.getFilePath(i));
				strcpy_s(pBuffer, pBufferEnd - pBuffer, "</Input>\n");
				pBuffer += strlen("</Input>\n");
			}

			pApp->getOutputFiles(OutputFileList);
			for (int i = 0; i < OutputFileList.getNumFilePaths(); i++)
			{
				strcpy_s(pBuffer, pBufferEnd - pBuffer, "<Output>");
				pBuffer += strlen("<Output>");
				strcpy_s(pBuffer, pBufferEnd - pBuffer, OutputFileList.getFilePath(i));
				pBuffer += strlen(OutputFileList.getFilePath(i));
				strcpy_s(pBuffer, pBufferEnd - pBuffer, "</Output>\n");
				pBuffer += strlen("</Output>\n");
			}
			strcpy_s(pBuffer, pBufferEnd - pBuffer, "</Files>");
			pBuffer += strlen("</Files>") + 1;


			delete pApp;
			return 0;
		}
		else throw std::exception("Wrong experiment configuration file");
	}
	catch (std::exception& e)
	{
		CLogger::logMessage(MessageType::Error, e.what());
		return -1;
	}
}
