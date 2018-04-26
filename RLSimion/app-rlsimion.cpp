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
#include "../tools/OpenGLRenderer/basic-shapes-2d.h"
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
				updateScene(s, a);

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

	//resize the main viewport
	double mainViewPortMinX = 0.0, mainViewPortMinY = 0.3, mainViewPortMaxX = 0.7, mainViewPortMaxY = 1.0;
	ViewPort* pMainViewPort= m_pRenderer->getDefaultViewPort();
	pMainViewPort->resize(mainViewPortMinX, mainViewPortMinY, mainViewPortMaxX, mainViewPortMaxY);
	m_pRenderer->setDataFolder(sceneDir);
	m_pRenderer->loadScene(sceneFile.c_str());

	//text
	m_pProgressText = new Text2D(string("Progress"), Vector2D(0.05, 0.95), 0.25);
	m_pRenderer->add2DGraphicObject(m_pProgressText);

	//create a second viewport for stats, state variables and action variables
	double metersViewPortMinX = mainViewPortMaxX, metersViewPortMinY = mainViewPortMinY, metersViewPortMaxX = 1.0, metersViewPortMaxY = 1.0;
	ViewPort* pMetersViewPort = m_pRenderer->addViewPort(metersViewPortMinX, metersViewPortMinY, metersViewPortMaxX, metersViewPortMaxY);

	//stats
	Meter2D* pMeter2D;
	IStats* pStat;
	Vector2D origin = Vector2D(0.025, 0.9);
	Vector2D size = Vector2D(0.95, 0.04);
	Vector2D offset = Vector2D(0.0, 0.05);
	double depth = 0.25;
	for (unsigned int i = 0; i < pLogger->getNumStats(); ++i)
	{
		pStat = pLogger->getStats(i);
		pMeter2D = new Meter2D(string(pStat->getSubkey()), origin, size, depth);
		m_pStatsUIMeters.push_back(pMeter2D);
		m_pRenderer->add2DGraphicObject(pMeter2D, pMetersViewPort);
		origin -= offset;
	}
	
	//state variables
	Descriptor& stateDescriptor = pWorld->getDynamicModel()->getStateDescriptor();
	for (unsigned int i = 0; i < stateDescriptor.size(); i++)
	{
		pMeter2D = new Meter2D(stateDescriptor[i].getName(), origin, size, depth);
		pMeter2D->setValueRange(Range(stateDescriptor[i].getMin(), stateDescriptor[i].getMax()));
		m_pStateUIMeters.push_back(pMeter2D);
		m_pRenderer->add2DGraphicObject(pMeter2D, pMetersViewPort);
		origin -= offset;
	}
	//action variables
	Descriptor& actionDescriptor = pWorld->getDynamicModel()->getActionDescriptor();
	for (unsigned int i = 0; i < actionDescriptor.size(); i++)
	{
		pMeter2D = new Meter2D(actionDescriptor[i].getName(), origin, size, depth);
		pMeter2D->setValueRange(Range(actionDescriptor[i].getMin(), actionDescriptor[i].getMax()));
		m_pActionUIMeters.push_back(pMeter2D);
		m_pRenderer->add2DGraphicObject(pMeter2D, pMetersViewPort);
		origin -= offset;
	}
	m_pInputHandler = new FreeCameraInputHandler();
	
	m_timer.start();
}

void RLSimionApp::updateScene(State* s, Action* a)
{
	//check the renderer has been initialized
	if (!m_pRenderer) return;

	//update progress text
	m_pProgressText->set(string("Episode: ") + std::to_string(pExperiment->getEpisodeIndex())
		+ string(" Step: ") + std::to_string(pExperiment->getStep()));

	//2D METERS
	//update stats
	unsigned int statIndex = 0;
	for each (Meter2D* pMeter2D in m_pStatsUIMeters)
	{
		IStats* pStat = pLogger->getStats(statIndex);
		pMeter2D->setValue(pStat->get());
		pMeter2D->setValueRange(Range(pStat->getStatsInfo()->getMin(), pStat->getStatsInfo()->getMax()));
		++statIndex;
	}
	//update state
	for (int i= 0; i < s->getNumVars(); i++)
		m_pStateUIMeters[i]->setValue(s->get(i));

	//update action
	for (int i = 0; i < a->getNumVars(); i++)
		m_pActionUIMeters[i]->setValue(a->get(i));

	//2D/3D BOUND PROPERTIES: translation, rotation, ...
	//update bindings
	double value;
	string varName;
	for (unsigned int b = 0; b < m_pRenderer->getNumBindings(); ++b)
	{
		varName = m_pRenderer->getBindingExternalName(b);
		value = s->get(varName.c_str());
		m_pRenderer->updateBinding(varName, value);
	}
	//drawViewPorts
	m_pInputHandler->handleInput();
	m_pRenderer->draw();

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