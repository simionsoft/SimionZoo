#include "stdafx.h"
#include "globals.h"
#include "world.h"
#include "experiment.h"
#include "SimGod.h"


CWorld* RLSimion::g_pWorld;
CExperiment *RLSimion::g_pExperiment;
CSimGod *RLSimion::g_pSimGod;

void RLSimion::init(tinyxml2::XMLElement* pParameters)
{
	//First, the world was created
	//No constructor in the world can call to any method from the CExperiment (i.e. stats) or CSimGod
	if (pParameters->FirstChildElement("World"))
		g_pWorld = new CWorld(pParameters->FirstChildElement("World"));
	else g_pWorld = 0;
	//Then, the experiment.
	//Dependency: it needs DT from the world to calculate the number of steps-per-episode
	if (pParameters->FirstChildElement("Experiment"))
		g_pExperiment = new CExperiment(pParameters->FirstChildElement("Experiment"));
	else g_pExperiment = 0;
	//Last, the SimGod was created
	//It can call from constructors any method from CWorld / CExperiment
	if (pParameters->FirstChildElement("SimGod"))
		g_pSimGod = new CSimGod(pParameters->FirstChildElement("SimGod"));
	else g_pSimGod = 0;
}

void RLSimion::shutdown()
{
	if (g_pWorld) delete g_pWorld;
	if (g_pExperiment) delete g_pExperiment;
	if (g_pSimGod) delete g_pSimGod;
}