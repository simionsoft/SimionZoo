#include "stdafx.h"
#include "app-rlsimion.h"
#include "worlds/world.h"
#include "experiment.h"
#include "SimGod.h"
#include "config.h"
#include "logger.h"
#include "named-var-set.h"
#include "utils.h"
#include "app.h"
#include "stats.h"
#include "../tools/OpenGLRenderer/renderer.h"
#include "../tools/OpenGLRenderer/text.h"
#include "../tools/OpenGLRenderer/input-handler.h"
#include "../tools/WindowsUtils/FileUtils.h"

RLSimionApp::RLSimionApp(ConfigNode* pConfigNode)
{
	pConfigNode = pConfigNode->getChild("RLSimion");
	if (!pConfigNode) throw std::exception("Wrong experiment configuration file");

	pMemManager = new MemManager<SimionMemPool>();

	//In the beginning, a logger was created so that we could tell about creation itself
	pLogger= CHILD_OBJECT<Logger>(pConfigNode, "Log", "The logger class");

	//Then the world was created by sheer chance
	pWorld = CHILD_OBJECT<World>(pConfigNode, "World", "The simulation environment and its parameters");

	//Then, the experiment.
	//Dependency: it needs DT from the world to calculate the number of steps-per-episode
	pExperiment = CHILD_OBJECT<Experiment>(pConfigNode, "Experiment", "The parameters of the experiment");

	//Last, the SimGod was created to create and control all the simions
	pSimGod= CHILD_OBJECT<SimGod>(pConfigNode,"SimGod"
		, "The omniscient class that controls all aspects of the simulation process");
}

RLSimionApp::~RLSimionApp()
{
	if (pMemManager != nullptr) delete pMemManager;
	if (m_pRenderer != nullptr) delete m_pRenderer;
}

void RLSimionApp::run()
{
	Logger::logMessage(MessageType::Info, "Simulation starting");
	SimionApp* pApp = SimionApp::get();

	//create state and action vectors
	State *s = pApp->pWorld->getDynamicModel()->getStateDescriptor().getInstance();
	State *s_p = pApp->pWorld->getDynamicModel()->getStateDescriptor().getInstance();
	Action *a = pApp->pWorld->getDynamicModel()->getActionDescriptor().getInstance();

	double r;
	double probability;
	pApp->pLogger->addVarToStats<double>("reward", "r", r);

	if (!m_bRemoteExecution)
	{
		string sceneFile = pApp->pWorld->getDynamicModel()->getWorldSceneFile();
		initRenderer(sceneFile);
	}

	//load stuff we don't want to be loaded in the constructors for faster construction
	pApp->pSimGod->deferredLoad();
	Logger::logMessage(MessageType::Info, "Deferred load step finished. Simulation starts");

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

			//log tuple <s,a,s',r> and stats
			pApp->pExperiment->timestep(s, a, s_p, pApp->pWorld->getRewardVector());
			//we need the complete reward vector for logging

			pApp->pSimGod->postUpdate();

			if (!m_bRemoteExecution)
				updateScene(s_p);

			//s= s'
			s->copy(s_p);
		}
	}
	Logger::logMessage(MessageType::Info, "Simulation finished");

	delete s;
	delete s_p;
	delete a;
}

void RLSimionApp::initRenderer(string sceneFile)
{
	char arguments[] = "RLSimion";
	char* argv = arguments;

	//initialize the render if a scene file can be found
	string sceneDir = "../config/scenes/";
	if (!bFileExists(sceneDir + sceneFile))
		return;

	m_pRenderer = new Renderer();
	m_pRenderer->init(1, &argv, 800, 600);
	m_pRenderer->setDataFolder(sceneDir);
	m_pRenderer->loadScene(sceneFile.c_str());

	//text
	m_pProgressText = new Text2D(string("Progress"), Vector2D(0.05, 0.95), 0);
	m_pRenderer->add2DGraphicObject(m_pProgressText);

	//stats
	Meter2D* pStatText;
	IStats* pStat;
	Vector2D origin = Vector2D(0.05, 0.8);
	Vector2D size = Vector2D(0.35, 0.05);
	for (unsigned int i = 0; i < pLogger->getNumStats(); ++i)
	{
		pStat = pLogger->getStats(i);
		pStatText = new Meter2D(string(pStat->getSubkey()), origin, size);
		m_pStatsText.push_back(pStatText);
		m_pRenderer->add2DGraphicObject(pStatText);
		origin -= Vector2D(0.0, 0.06);
	}

	m_pInputHandler = new FreeCameraInputHandler();
	
	m_timer.start();
}

void RLSimionApp::updateScene(State* s)
{
	//check the renderer has been initialized
	if (!m_pRenderer) return;

	//update progress text
	m_pProgressText->set(string("Episode: ") + std::to_string(pExperiment->getEpisodeIndex())
		+ string(" Step: ") + std::to_string(pExperiment->getStep()));

	//update stats
	IStats* pStat;
	unsigned int statIndex = 0;
	for (auto it= m_pStatsText.begin(); it!=m_pStatsText.end(); ++it)
	{
		pStat = pLogger->getStats(statIndex);
		(*it)->setValue(pStat->get());
		(*it)->setValueRange(Range(pStat->getStatsInfo()->getMin(), pStat->getStatsInfo()->getMax()));
		++statIndex;
	}

	//update bindings
	double value;
	string varName;
	for (unsigned int b = 0; b < m_pRenderer->getNumBindings(); ++b)
	{
		varName = m_pRenderer->getBindingExternalName(b);
		value = s->get(varName.c_str());
		m_pRenderer->updateBinding(varName, value);
	}
	//drawScene
	m_pInputHandler->handleInput();
	m_pRenderer->redraw();

	//real time execution?
	if (!((FreeCameraInputHandler*)m_pInputHandler)->getRealTimeExecutionDisabled())
	{
		double dt = pWorld->getDT();
		double elapsedTime = m_timer.getElapsedTime(true);
		if (dt > elapsedTime)
		{
			Sleep((unsigned long)(1000.0*(dt - elapsedTime)));
			m_timer.start();
		}
	}
}