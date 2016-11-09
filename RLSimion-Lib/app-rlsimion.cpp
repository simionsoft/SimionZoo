#include "stdafx.h"
#include "app-rlsimion.h"
#include "world.h"
#include "experiment.h"
#include "SimGod.h"
#include "config.h"
#include "logger.h"
#include "named-var-set.h"
#include "globals.h"
#include "utils.h"
#include "app.h"

void RLSimionApp::getOutputFiles(CFilePathList& filePathList)
{
	SimGod.getOutputFiles(filePathList);
}

void RLSimionApp::getInputFiles(CFilePathList& filePathList)
{
	SimGod.getInputFiles(filePathList);
}

CLASS_CONSTRUCTOR(RLSimionApp, const char* xmlConfigPath)
{
	pParameters = pParameters->getChild("RLSimion");
	if (!pParameters) throw std::exception("Wrong experiment configuration file");


	//In the beginning, a logger was created so that we could know about creation itself
	CHILD_CLASS(pLogger, "Log", "The logger class", false, CLogger);
	pLogger->setLogDirectory(xmlConfigPath); //we provide the path to the xml configuration file so that the logger saves its log files in the directory

	//Then the world was created by chance
	CHILD_CLASS(pWorld, "World", "The simulation environment and its parameters", false, CWorld);

	//Then, the experiment.
	CHILD_CLASS(pExperiment, "Experiment", "The parameters of the experiment", false, CExperiment);	//Dependency: it needs DT from the world to calculate the number of steps-per-episode

	//Last, the SimGod was created to create and control all the simions
	CHILD_CLASS_INIT(SimGod, "SimGod", "The god class that controls all aspects of the simulation process", false, CSimGod);


	END_CLASS();
}

RLSimionApp::~RLSimionApp()
{
}

void RLSimionApp::run()
{
	CLogger::logMessage(MessageType::Info, "Simulation starting");
	CApp* pApp = CApp::get();

	//create state and action vectors
	CState *s = pApp->pWorld->getDynamicModel()->getStateDescriptor()->getInstance();
	CState *s_p = pApp->pWorld->getDynamicModel()->getStateDescriptor()->getInstance();
	CAction *a = pApp->pWorld->getDynamicModel()->getActionDescriptor()->getInstance();

	//load stuff we don't want to be loaded in the constructors for faster construction
	pApp->SimGod.delayedLoad();
	CLogger::logMessage(MessageType::Info, "Deferred load step finished. Simulation starts");

	double r = 0.0;

	//episodes
	for (pApp->pExperiment->nextEpisode(); pApp->pExperiment->isValidEpisode(); pApp->pExperiment->nextEpisode())
	{
		pApp->pWorld->reset(s);

		//steps per episode
		for (pApp->pExperiment->nextStep(); pApp->pExperiment->isValidStep(); pApp->pExperiment->nextStep())
		{
			//a= pi(s)
			pApp->SimGod.selectAction(s, a);

			//s_p= f(s,a); r= R(s');
			r = pApp->pWorld->executeAction(s, a, s_p);

			//update god's policy and value estimation
			pApp->SimGod.update(s, a, s_p, r);

			//log tuple <s,a,s',r>
			pApp->pExperiment->timestep(s, a, s_p, pApp->pWorld->getRewardVector());
			//we need the complete reward vector for logging

			//s= s'
			s->copy(s_p);
		}
	}
	CLogger::logMessage(MessageType::Info, "Simulation finished");

	delete s;
	delete s_p;
	delete a;
}
