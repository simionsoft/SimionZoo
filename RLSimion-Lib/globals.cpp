#include "stdafx.h"
#include "globals.h"
#include "world.h"
#include "experiment.h"
#include "SimGod.h"
#include "parameters.h"
#include "logger.h"

CLogger *RLSimion::g_pLogger= 0;
CWorld* RLSimion::g_pWorld= 0;
CExperiment *RLSimion::g_pExperiment= 0;
CSimGod *RLSimion::g_pSimGod= 0;

ENUMERATION(Boolean, "False", "True");
ENUMERATION(Distribution, "linear", "quadratic", "cubic");
ENUMERATION(Interpolation, "linear", "quadratic", "cubic");
ENUMERATION(TimeReference, "episode", "experiment", "experimentTime");

void RLSimion::init(CParameters* pParameters)
{
	//First, a logger was created so that we could know about creation itself
	g_pLogger = new CLogger(pParameters->getChild("Log")); //with or without parameters
	
	//Then the world was created by chance
	if (pParameters->getChild("World"))
		g_pWorld = new CWorld(pParameters->getChild("World"));
	else g_pWorld = 0;

	//Then, the experiment.
	g_pExperiment = new CExperiment(pParameters->getChild("Experiment"));	//Dependency: it needs DT from the world to calculate the number of steps-per-episode

	//Last, the SimGod was created to create and control all the simions
	if (pParameters->getChild("SimGod"))
		g_pSimGod = new CSimGod(pParameters->getChild("SimGod"));
	else g_pSimGod = 0;
}

void RLSimion::shutdown()
{
	if (g_pLogger) 
	{ 
		delete g_pLogger;
		g_pLogger = 0;
	}
	if (g_pWorld) 
	{
		delete g_pWorld;
		g_pWorld = 0;
	}
	if (g_pExperiment)
	{
		delete g_pExperiment;
		g_pExperiment = 0;
	}
	if (g_pSimGod)
	{
		delete g_pSimGod;
		g_pSimGod = 0;
	}
}