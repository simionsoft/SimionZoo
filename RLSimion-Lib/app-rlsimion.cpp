#include "stdafx.h"
#include "app-rlsimion.h"
#include "world.h"
#include "experiment.h"
#include "SimGod.h"
#include "parameters.h"
#include "logger.h"
#include "named-var-set.h"
#include "globals.h"
#include "utils.h"

void RLSimionApp::getOutputFiles(CFilePathList& filePathList)
{
	//the list of log files depends on the experiment parameters and the logger parameters, so instead of introducing a dependency between them
	//i thought it best to code it here
	char filepath[CFilePathList::m_filePathMaxSize];
	bool bEvaluation;

	//episode log files
	for (CApp::Experiment.nextEpisode(); CApp::Experiment.isValidEpisode(); CApp::Experiment.nextEpisode())
	{
		bEvaluation = Experiment.isEvaluationEpisode();
		if (Logger.isEpisodeTypeLogged(bEvaluation))
			Logger.getLogFilename(filepath, CFilePathList::m_filePathMaxSize, false, bEvaluation,Experiment.getRelativeEpisodeIndex());
		filePathList.addFilePath(filepath);
	}
	//experiment log files
	bool bExperimentType = true;
	if (Logger.isExperimentTypeLogged(bExperimentType))
	{
		Logger.getLogFilename(filepath, CFilePathList::m_filePathMaxSize, false, bExperimentType, 0);
		filePathList.addFilePath(filepath);
	}
	bExperimentType = false;
	if (Logger.isExperimentTypeLogged(bExperimentType))
	{
		Logger.getLogFilename(filepath, CFilePathList::m_filePathMaxSize, false, bExperimentType, 0);
		filePathList.addFilePath(filepath);
	}
	SimGod.getOutputFiles(filePathList);
}

void RLSimionApp::getInputFiles(CFilePathList& filePathList)
{
	SimGod.getInputFiles(filePathList);
}

APP_CLASS(RLSimionApp)
{
	CParameters* pParameters = m_pConfigDoc->loadFile(argv[1], "RLSimion");
	if (!pParameters) throw std::exception("Wrong experiment configuration file");
	pParameters = pParameters->getChild("RLSimion");
	if (!pParameters) throw std::exception("Wrong experiment configuration file");


	//In the beginning, a logger was created so that we could know about creation itself
	CHILD_CLASS_INIT(Logger, "Log", "The logger class", false, CLogger);
	Logger.setLogDirectory(argv[1]); //we provide the path to the xml configuration file so that the logger saves its log files in the directory

	//Then the world was created by chance
	CHILD_CLASS_INIT(World, "World", "The simulation environment and its parameters", false, CWorld);

	//Then, the experiment.
	CHILD_CLASS_INIT(Experiment, "Experiment", "The parameters of the experiment", false, CExperiment);	//Dependency: it needs DT from the world to calculate the number of steps-per-episode

	//Last, the SimGod was created to create and control all the simions
	CHILD_CLASS_INIT(SimGod, "SimGod", "The god class that controls all aspects of the simulation process", false, CSimGod);


	END_CLASS();
}

RLSimionApp::~RLSimionApp()
{
}

void RLSimionApp::run()
{
	//create state and action vectors
	CState *s = CApp::World.getDynamicModel()->getStateDescriptor()->getInstance();
	CState *s_p = CApp::World.getDynamicModel()->getStateDescriptor()->getInstance();
	CAction *a = CApp::World.getDynamicModel()->getActionDescriptor()->getInstance();
	//register the state and action vectors in the logger
	CApp::Logger.addVarSetToStats("State", s);
	CApp::Logger.addVarSetToStats("Action", a);
	CApp::Logger.addVarToStats("Reward", "sum", CApp::World.getScalarReward());
	CApp::Logger.addVarSetToStats("Reward", CApp::World.getReward());

	//load stuff we don't want to be loaded in the constructors for faster construction
	CApp::SimGod.delayedLoad();

	double r = 0.0;

	//episodes
	for (CApp::Experiment.nextEpisode(); CApp::Experiment.isValidEpisode(); CApp::Experiment.nextEpisode())
	{
		CApp::World.reset(s);

		//steps per episode
		for (CApp::Experiment.nextStep(); CApp::Experiment.isValidStep(); CApp::Experiment.nextStep())
		{
			//a= pi(s)
			CApp::SimGod.selectAction(s, a);

			//s_p= f(s,a); r= R(s');
			r = CApp::World.executeAction(s, a, s_p);

			//update god's policy and value estimation
			CApp::SimGod.update(s, a, s_p, r);

			//log tuple <s,a,s',r>
			CApp::Experiment.timestep(s, a, s_p, CApp::World.getReward()); //we need the complete reward vector for logging

			//s= s'
			s->copy(s_p);
		}
	}

	delete s;
	delete s_p;
	delete a;
}
