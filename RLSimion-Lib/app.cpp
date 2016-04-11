#include "stdafx.h"
#include "app.h"
#include "logger.h"
#include "world.h"
#include "experiment.h"
#include "simgod.h"
#include "parameters.h"
#include "globals.h"
#include "app-rlsimion.h"
#include "app-controller-to-vfa.h"

CLogger CApp::Logger;
CWorld CApp::World;
CExperiment CApp::Experiment;
CSimGod CApp::SimGod;

CApp::CApp(int argc, char* argv[])
{
	//minimal initialisation required for all apps: pipe, load the xml configuration file, ....
	if (argc > 2)
		CLogger::createOutputPipe(argv[2]);

	if (argc <= 1)
		Logger.logMessage(MessageType::Error, "Too few parameters");

	m_pConfigDoc = new CParameterFile;
}



CApp::~CApp()
{
	if (m_pConfigDoc)
	{
		delete m_pConfigDoc; m_pConfigDoc = 0;
	}
	CLogger::closeOutputPipe();
}
