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
	if (pParameters->FirstChildElement("World"))
		g_pWorld = new CWorld(pParameters->FirstChildElement("World"));
	else g_pWorld = 0;
	if (pParameters->FirstChildElement("Experiment"))
		g_pExperiment = new CExperiment(pParameters->FirstChildElement("Experiment"));
	else g_pExperiment = 0;
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