#include "stdafx.h"
#include "globals.h"
#include "world.h"
#include "experiment.h"
#include "SimGod.h"
#include "parameters.h"
#include "logger.h"


CLogger RLSimion::Logger;
CWorld RLSimion::World;
CExperiment RLSimion::Experiment;
CSimGod RLSimion::SimGod;
CParameterFile* g_pConfigDoc= 0;

ENUMERATION(Boolean, "False", "True");
ENUMERATION(Distribution, "linear", "quadratic", "cubic");
ENUMERATION(Interpolation, "linear", "quadratic", "cubic");
ENUMERATION(TimeReference, "episode", "experiment", "experimentTime");



CParameters* RLSimion::init(int argc, char* argv[],const char* rootNodeName)
{
	CParameters* pParameters;

	if (!g_pConfigDoc) g_pConfigDoc = new CParameterFile;

	if (argc > 1) pParameters = g_pConfigDoc->loadFile(argv[1], rootNodeName);

	if (argc <= 1 || !pParameters)
	{
		getchar();
		exit(-1);
	}

	if (argc > 2) CLogger::createOutputPipe(argv[2]);

	//In the beginning, a logger was created so that we could know about creation itself
	Logger.init(pParameters->getChild("Log")); //with or without parameters
	
	//Then the world was created by chance
	World.init(pParameters->getChild("World"));

	//Then, the experiment.
	Experiment.init(pParameters->getChild("Experiment"));	//Dependency: it needs DT from the world to calculate the number of steps-per-episode

	//Last, the SimGod was created to create and control all the simions
	SimGod.init(pParameters->getChild("SimGod"));

	return pParameters;
}

void RLSimion::shutdown()
{
	if (g_pConfigDoc)
	{
		delete g_pConfigDoc;
		g_pConfigDoc = 0;
	}
	CLogger::closeOutputPipe();
}