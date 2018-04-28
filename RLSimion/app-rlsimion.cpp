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
#include "function-sampler.h"
#include "state-action-function.h"
#include "../tools/OpenGLRenderer/basic-shapes-2d.h"
#include "../tools/OpenGLRenderer/renderer.h"
#include "../tools/OpenGLRenderer/text.h"
#include "../tools/OpenGLRenderer/input-handler.h"
#include "../tools/OpenGLRenderer/material.h"
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

	//create state and action vectors
	State *s = pWorld->getDynamicModel()->getStateDescriptor().getInstance();
	State *s_p = pWorld->getDynamicModel()->getStateDescriptor().getInstance();
	Action *a = pWorld->getDynamicModel()->getActionDescriptor().getInstance();

	double r;
	double probability;
	pLogger->addVarToStats<double>("reward", "r", r);

	//load stuff we don't want to be loaded in the constructors for faster construction
	pSimGod->deferredLoad();
	Logger::logMessage(MessageType::Info, "Deferred load step finished");

	//load the scene and initialize visual objects
	if (!m_bRemoteExecution)
	{
		//load the scene
		string sceneFile = pWorld->getDynamicModel()->getWorldSceneFile();
		initRenderer(sceneFile, s, a);
	}
	Logger::logMessage(MessageType::Info, "Simulation begins");

	//episodes
	for (pExperiment->nextEpisode(); pExperiment->isValidEpisode(); pExperiment->nextEpisode())
	{
		pWorld->reset(s);

		//steps per episode
		for (pExperiment->nextStep(); pExperiment->isValidStep(); pExperiment->nextStep())
		{
			//a= pi(s)
			probability= pSimGod->selectAction(s, a);

			//s_p= f(s,a); r= R(s');
			r = pWorld->executeAction(s, a, s_p);

			//update god's policy and value estimation
			pSimGod->update(s, a, s_p, r, probability);

			//log tuple <s,a,s',r> and stats
			pExperiment->timestep(s, a, s_p, pWorld->getRewardVector());
			//we need the complete reward vector for logging

			pSimGod->postUpdate();

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

void RLSimionApp::initRenderer(string sceneFile, State* s, Action* a)
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

	//create function samplers
	size_t numFunctions = m_pStateActionFunctions.size();
	if (numFunctions)
	{
		double functionViewPortMinX = 0.0, functionViewPortMinY = mainViewPortMinX
			, functionViewPortMaxX = 1.0, functionViewPortMaxY = mainViewPortMinY;
		double viewSizeInX = std::min(0.25, 1.0 / numFunctions);
		double totalSizeInX = viewSizeInX * numFunctions;
		double xOffset = 0.0075, yOffset = 0.02;
		Vector2D functionViewOrigin = Vector2D((1 - totalSizeInX) *0.5 + xOffset, yOffset);
		Vector2D functionViewSize = Vector2D(viewSizeInX - 2*xOffset, 1.0 - 2*yOffset);
		Vector2D legendOffset = Vector2D(0.01, 0.02);
		ViewPort* functionViewPort = new ViewPort(functionViewPortMinX, functionViewPortMinY, functionViewPortMaxX
			, functionViewPortMaxY);
		m_pRenderer->addViewPort(functionViewPort);
		for (auto functionIt : m_pStateActionFunctions)
		{
			if (functionIt.second->getInputActionVariables().size() + functionIt.second->getInputStateVariables().size() > 1)
			{
				//create the sampler: it will transform the state-action function to a 2D texture
				FunctionSampler* pSampler = new FunctionSampler3D(functionIt.second, m_numSamplesPerDim
					, s->getDescriptor(), a->getDescriptor());
				//create the live material. We can update its associated texture
				UnlitLiveTextureMaterial* pLiveMaterial = new UnlitLiveTextureMaterial(m_numSamplesPerDim, m_numSamplesPerDim);
				m_pFunctionViews[pLiveMaterial] = pSampler;

				//create the sprite with the live texture attached to it
				Sprite2D* pFunctionViewer = new Sprite2D(functionIt.first + string("-view"), functionViewOrigin, functionViewSize, 0.25, pLiveMaterial);
				m_pRenderer->add2DGraphicObject(pFunctionViewer, functionViewPort);
				Text2D* pFunctionLegend = new Text2D(functionIt.first + string("-legend"), functionViewOrigin + legendOffset, 0.3);
				pFunctionLegend->set(functionIt.first);
				pFunctionLegend->setColor(Color(1.f, 1.f, 1.f)); //white texts over red-blue function views
				m_pRenderer->add2DGraphicObject(pFunctionLegend, functionViewPort);

				//shift the origin x coordinate to the right
				functionViewOrigin.setX(functionViewOrigin.x() + functionViewSize.x() + 2*xOffset);
			}
		}
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

	update2DMeters(s, a);

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

	//Update the function views each m_functionViewUpdateStepFreq steps
	if (pExperiment->getStep() % m_functionViewUpdateStepFreq == 0)
	{
		for each(auto functionIt in m_pFunctionViews)
		{
			const vector<double>& sampledValues = functionIt.second->sample();
			functionIt.first->updateTexture(sampledValues);
		}
	}
	//render the image
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

void RLSimionApp::update2DMeters(State* s, Action* a)
{
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
	for (int i = 0; i < s->getNumVars(); i++)
		m_pStateUIMeters[i]->setValue(s->get(i));

	//update action
	for (int i = 0; i < a->getNumVars(); i++)
		m_pActionUIMeters[i]->setValue(a->get(i));
}