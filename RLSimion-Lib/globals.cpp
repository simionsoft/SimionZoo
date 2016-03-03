#include "stdafx.h"
#include "globals.h"
#include "world.h"
#include "experiment.h"
#include "SimGod.h"
#include "parameters.h"

CWorld* RLSimion::g_pWorld;
CExperiment *RLSimion::g_pExperiment;
CSimGod *RLSimion::g_pSimGod;

void RLSimion::init(CParameters* pParameters)
{
	//First, the world was created
	//No constructor in the world can call to any method from the CExperiment (i.e. stats) or CSimGod
	if (pParameters->getChild("World"))
		g_pWorld = new CWorld(pParameters->getChild("World"));
	else g_pWorld = 0;
	//Then, the experiment.
	//Dependency: it needs DT from the world to calculate the number of steps-per-episode
	if (pParameters->getChild("Experiment"))
		g_pExperiment = new CExperiment(pParameters->getChild("Experiment"));
	else g_pExperiment = 0;
	//Last, the SimGod was created
	//It can call from constructors any method from CWorld / CExperiment
	if (pParameters->getChild("SimGod"))
		g_pSimGod = new CSimGod(pParameters->getChild("SimGod"));
	else g_pSimGod = 0;
}

void RLSimion::shutdown()
{
	if (g_pWorld) delete g_pWorld;
	if (g_pExperiment) delete g_pExperiment;
	if (g_pSimGod) delete g_pSimGod;
	CParameters::freeHandlers();
}