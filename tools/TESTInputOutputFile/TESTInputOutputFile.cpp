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
	CFilePathList IOFileList;
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
			pApp->getInputFiles(IOFileList);

			pApp->getOutputFiles(IOFileList);
			for (int i = 0; i < IOFileList.getNumFilePaths(); i++)
				printf("%s\n", IOFileList.getFilePath(i));


			delete pApp;
		}
		else throw std::exception("Wrong experiment configuration file");
	}
	catch (std::exception& e)
	{
		CLogger::logMessage(MessageType::Error, e.what());
	}
}

