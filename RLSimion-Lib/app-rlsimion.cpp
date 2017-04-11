#include "stdafx.h"
#include "app-rlsimion.h"
#include "world.h"
#include "experiment.h"
#include "SimGod.h"
#include "config.h"
#include "logger.h"
#include "named-var-set.h"
#include "utils.h"
#include "app.h"

void RLSimionApp::getOutputFiles(CFilePathList& filePathList)
{
	pSimGod->getOutputFiles(filePathList);
}

void RLSimionApp::getInputFiles(CFilePathList& filePathList)
{
	pSimGod->getInputFiles(filePathList);
}

void RLSimionApp::setOutputDirectory(const char* pOutputDir)
{
	//we provide the path to the xml configuration file so that the logger saves its log files in the directory
	strcpy_s(m_directory, MAX_PATH_SIZE, pOutputDir);

	int i = strlen(m_directory) - 1;
	while (i > 0 && m_directory[i] != '/' && m_directory[i] != '\\')
		i--;

	if (i > 0) m_directory[i] = 0;

	pLogger->setLogDirectory(m_directory);
}

char* RLSimionApp::getOutputDirectory()
{
	return m_directory;
}

RLSimionApp::RLSimionApp(CConfigNode* pConfigNode)
{
	pConfigNode = pConfigNode->getChild("RLSimion");
	if (!pConfigNode) throw std::exception("Wrong experiment configuration file");

	//In the beginning, a logger was created so that we could tell about creation itself
	pLogger= CHILD_OBJECT<CLogger>(pConfigNode, "Log", "The logger class");

	//Then the world was created by sheer chance
	pWorld = CHILD_OBJECT<CWorld>(pConfigNode, "World", "The simulation environment and its parameters");

	//Then, the experiment.
	//Dependency: it needs DT from the world to calculate the number of steps-per-episode
	pExperiment = CHILD_OBJECT<CExperiment>(pConfigNode, "Experiment", "The parameters of the experiment");

	//Last, the SimGod was created to create and control all the simions
	pSimGod= CHILD_OBJECT<CSimGod>(pConfigNode,"SimGod"
		, "The omniscient class that controls all aspects of the simulation process");
}

RLSimionApp::~RLSimionApp()
{
}

void RLSimionApp::run()
{
	CLogger::logMessage(MessageType::Info, "Simulation starting");
	CSimionApp* pApp = CSimionApp::get();

	//create state and action vectors
	CState *s = pApp->pWorld->getDynamicModel()->getStateDescriptor().getInstance();
	CState *s_p = pApp->pWorld->getDynamicModel()->getStateDescriptor().getInstance();
	CAction *a = pApp->pWorld->getDynamicModel()->getActionDescriptor().getInstance();

	//load stuff we don't want to be loaded in the constructors for faster construction
	pApp->pSimGod->deferredLoad();
	CLogger::logMessage(MessageType::Info, "Deferred load step finished. Simulation starts");

	double r;
	double probability;

	//episodes
	for (pApp->pExperiment->nextEpisode(); pApp->pExperiment->isValidEpisode(); pApp->pExperiment->nextEpisode())
	{
		pApp->pWorld->reset(s);

		//steps per episode
		for (pApp->pExperiment->nextStep(); pApp->pExperiment->isValidStep(); pApp->pExperiment->nextStep())
		{
			//a= pi(s)
			probability= pApp->pSimGod->selectAction(s, a);

			//s_p= f(s,a); r= R(s');
			r = pApp->pWorld->executeAction(s, a, s_p);

			//update god's policy and value estimation
			pApp->pSimGod->update(s, a, s_p, r, probability);

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
