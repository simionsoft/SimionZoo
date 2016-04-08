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

	if (!m_pConfigDoc)
		m_pConfigDoc = new CParameterFile;
}

CLASS_FACTORY(CApp,int argc,char* argv[])
{
	CHOICE("APP", "The application");
	CHOICE_ELEMENT("RLSimion", RLSimionApp, "Full simulation application",argc,argv);
	CHOICE_ELEMENT("ControllerApproximator", ControllerToVFAApp, "Approximates an existing controller using VFA", argc, argv);
	END_CHOICE();
	END_CLASS();
}

CApp::~CApp()
{
	if (m_pConfigDoc)
	{
		delete m_pConfigDoc; m_pConfigDoc = 0;
	}
	CLogger::closeOutputPipe();
}
