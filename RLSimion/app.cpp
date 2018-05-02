#include "stdafx.h"
#include "app.h"
#include "logger.h"
#include "worlds/world.h"
#include "experiment.h"
#include "SimGod.h"
#include "config.h"
#include "utils.h"
#include "function-sampler.h"
#include "state-action-function.h"
#include "../tools/OpenGLRenderer/basic-shapes-2d.h"
#include "../tools/OpenGLRenderer/renderer.h"
#include "../tools/OpenGLRenderer/text.h"
#include "../tools/OpenGLRenderer/input-handler.h"
#include "../tools/OpenGLRenderer/material.h"
#include "../tools/OpenGLRenderer/arranger.h"
#include "../tools/WindowsUtils/FileUtils.h"

SimionApp* SimionApp::m_pAppInstance = 0;

SimionApp::SimionApp(ConfigNode* pConfigNode)
{
	m_pAppInstance = this;

	pConfigNode = pConfigNode->getChild("RLSimion");
	if (!pConfigNode) throw std::exception("Wrong experiment configuration file");

	pMemManager = new MemManager<SimionMemPool>();

	//In the beginning, a logger was created so that we could tell about creation itself
	pLogger = CHILD_OBJECT<Logger>(pConfigNode, "Log", "The logger class");

	//Then the world was created by sheer chance
	pWorld = CHILD_OBJECT<World>(pConfigNode, "World", "The simulation environment and its parameters");

	//Then, the experiment.
	//Dependency: it needs DT from the world to calculate the number of steps-per-episode
	pExperiment = CHILD_OBJECT<Experiment>(pConfigNode, "Experiment", "The parameters of the experiment");

	//Last, the SimGod was created to create and control all the simions
	pSimGod = CHILD_OBJECT<SimGod>(pConfigNode, "SimGod"
		, "The omniscient class that controls all aspects of the simulation process");
}

SimionApp::~SimionApp()
{
	if (pMemManager != nullptr) delete pMemManager;
	if (m_pRenderer != nullptr) delete m_pRenderer;

	for each (FunctionSampler* sampler in m_pFunctionSamplers) delete sampler;

	m_pAppInstance = 0;
	for (auto it = m_inputFiles.begin(); it != m_inputFiles.end(); it++) delete (*it);
	m_inputFiles.clear();
	for (auto it = m_outputFiles.begin(); it != m_outputFiles.end(); it++) delete (*it);
	m_outputFiles.clear();
	for (auto it = m_inputFilesRenamed.begin(); it != m_inputFilesRenamed.end(); it++)
	{
		if (*it != 0)
			delete *it;
	}
	m_inputFilesRenamed.clear();
}

const char* SimionApp::getArgValue(int argc,char** argv,char* argName)
{
	char argPrefix[256];
	sprintf_s(argPrefix, 256, "-%s=", argName);
	for (int i = 1; i < argc; ++i)
	{
		if (strstr(argv[i], argPrefix) == argv[i])
		{
			return (char*)(argv[i]) + strlen(argPrefix);
		}
	}
	return nullptr;
}

bool SimionApp::flagPassed(int argc, char** argv, char* flagName)
{
	char argPrefix[256];
	sprintf_s(argPrefix, 256, "-%s", flagName);
	for (int i = 1; i < argc; ++i)
	{
		if (!strcmp(argv[i], argPrefix))
		{
			return true;
		}
	}
	return false;
}

void SimionApp::printRequirements()
{
	const char *pFileName, *pFileRename;
	printf("<Files>\n");

	for (unsigned int i = 0; i < getNumInputFiles(); i++)
	{
		pFileName = getInputFile(i);
		pFileRename = getInputFileRename(i);
		if (pFileRename==0)
			printf("  <Input>%s</Input>\n", pFileName);
		else
			printf("  <Input Rename=\"%s\">%s</Input>\n", pFileRename, pFileName);
	}

	for (unsigned int i = 0; i < getNumOutputFiles(); i++)
	{
		pFileName = getOutputFile(i);
		printf("  <Output>%s</Output>\n", pFileName);
	}
	printf("</Files>\n");
}

SimionApp* SimionApp::get()
{
	return m_pAppInstance;
}

void SimionApp::setExecutedRemotely(bool remote)
{
	m_bRemoteExecution = remote;
}

bool SimionApp::isExecutedRemotely()
{
	return m_bRemoteExecution;
}

void SimionApp::setConfigFile(string configFile)
{
	//we provide the path to the xml configuration file so that the logger saves its log files in the directory
	m_directory = getDirectory(configFile);
	m_configFile = configFile;

	pLogger->setOutputFilenames();
}

string SimionApp::getConfigFile()
{
	return m_configFile;
}

string SimionApp::getOutputDirectory()
{
	return m_directory;
}


void SimionApp::registerStateActionFunction(string name, StateActionFunction* pFunction)
{
	m_pStateActionFunctions[name] = pFunction;
}

void SimionApp::registerInputFile(const char* filepath, const char* rename)
{
	char* copy = new char[strlen(filepath) + 1];
	strcpy_s(copy, strlen(filepath) + 1, filepath);
	m_inputFiles.push_back(copy);
	if (rename != 0)
	{
		copy = new char[strlen(rename) + 1];
		strcpy_s(copy, strlen(rename) + 1, rename);
		m_inputFilesRenamed.push_back(copy);
	}
	else m_inputFilesRenamed.push_back(0);
}

unsigned int SimionApp::getNumInputFiles()
{
	return (unsigned int) m_inputFiles.size();
}

const char* SimionApp::getInputFile(unsigned int i)
{
	if (i<m_inputFiles.size())
		return m_inputFiles[i];
	return 0;
}

const char* SimionApp::getInputFileRename(unsigned int i)
{
	if (i<m_inputFilesRenamed.size())
		return m_inputFilesRenamed[i];
	return 0;
}

unsigned int SimionApp::getNumOutputFiles()
{
	return (unsigned int) m_outputFiles.size();
}

const char* SimionApp::getOutputFile(unsigned int i)
{
	if (i<m_outputFiles.size())
		return m_outputFiles[i];
	return 0;
}


void SimionApp::registerOutputFile(const char* filepath)
{
	char* copy = new char[strlen(filepath) + 1];
	strcpy_s(copy, strlen(filepath) + 1, filepath);
	m_outputFiles.push_back(copy);
}



void SimionApp::run()
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
	else
		if (pLogger->areFunctionsLogged())
			initFunctionSamplers(s, a);

	Logger::logMessage(MessageType::Info, "Simulation begins");

	//episodes
	for (pExperiment->nextEpisode(); pExperiment->isValidEpisode(); pExperiment->nextEpisode())
	{
		pWorld->reset(s);

		//steps per episode
		for (pExperiment->nextStep(); pExperiment->isValidStep(); pExperiment->nextStep())
		{
			//a= pi(s)
			probability = pSimGod->selectAction(s, a);

			//s_p= f(s,a); r= R(s');
			r = pWorld->executeAction(s, a, s_p);

			//update god's policy and value estimation
			pSimGod->update(s, a, s_p, r, probability);

			//log tuple <s,a,s',r> and stats
			pExperiment->timestep(s, a, s_p, pWorld->getRewardVector());
			//we need the complete reward vector for logging

			//do experience replay if enabled
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

void SimionApp::initRenderer(string sceneFile, State* s, Action* a)
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
	ViewPort* pMainViewPort = m_pRenderer->getDefaultViewPort();
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
	if (m_pStateActionFunctions.size())
	{
		initFunctionSamplers(s, a);

		ViewPort* functionViewPort = new ViewPort(0.0, 0.0, 1.0, mainViewPortMinY);
		m_pRenderer->addViewPort(functionViewPort);
		vector<GraphicObject2D*> objects;
		for each (FunctionSampler* pSampler in m_pFunctionSamplers)
		{
			//create the live material. We can update its associated texture
			UnlitLiveTextureMaterial* pLiveMaterial = new UnlitLiveTextureMaterial(m_numSamplesPerDim, m_numSamplesPerDim);
			m_pFunctionViews[pLiveMaterial] = pSampler;

			//create the sprite with the live texture attached to it
			Sprite2D* pFunctionViewer = new Sprite2D(pSampler->getFunctionId(), Vector2D(0.0, 0.0), Vector2D(0.0,0.0), 0.25, pLiveMaterial);
			m_pRenderer->add2DGraphicObject(pFunctionViewer, functionViewPort);
			objects.push_back(pFunctionViewer);
		}

		Arranger objectArranger;
		objectArranger.arrangeObjects2D(objects, Vector2D(0.0,0.0), Vector2D(1.0,1.0), Vector2D(0.15,0.25)
			, Vector2D(0.25, 1.0), Vector2D(0.0075, 0.02));

		//set legend to each function viewer
		int objIndex = 0;
		Vector2D legendOffset;
		for each (GraphicObject2D* pObj in objects)
		{
			if (objIndex % 2 == 0)
				legendOffset = Vector2D(0.01, 0.02);
			else
				legendOffset = Vector2D(0.01, pObj->getTransform().scale().y() - 0.1);
			Text2D* pFunctionLegend = new Text2D(pObj->name() + string("-legend"), pObj->getTransform().translation() + legendOffset, 0.3);
			pFunctionLegend->set(pObj->name());
			m_pRenderer->add2DGraphicObject(pFunctionLegend, functionViewPort);

			objIndex++;
		}
	}

	m_pInputHandler = new FreeCameraInputHandler();

	m_timer.start();
}

void SimionApp::initFunctionSamplers(State* s, Action* a)
{
	vector<pair<VariableSource, string>> m_sampledVariables;
	size_t numInputs;
	for (auto functionIt : m_pStateActionFunctions)
	{
		numInputs = functionIt.second->getInputActionVariables().size() + functionIt.second->getInputStateVariables().size();
		if (numInputs > 1)
		{
			//store all sampled variables to group them in pairs
			m_sampledVariables.clear();
			for each(string varName in functionIt.second->getInputStateVariables())
				m_sampledVariables.push_back(make_pair(VariableSource::StateSource, varName));
			for each(string varName in functionIt.second->getInputActionVariables())
				m_sampledVariables.push_back(make_pair(VariableSource::ActionSource, varName));

			//make all possible input variable combinations and one function sampler for each
			for (int i = 0; i < numInputs - 1; ++i)
			{
				for (int j = i + 1; j < numInputs; j++)
				{
					//create the sampler: it will transform the state-action function to a 2D texture
					FunctionSampler* pSampler = new FunctionSampler3D(functionIt.first, functionIt.second, m_numSamplesPerDim
						, s->getDescriptor(), a->getDescriptor(), m_sampledVariables[i].first, m_sampledVariables[i].second
						, m_sampledVariables[j].first, m_sampledVariables[j].second);
					m_pFunctionSamplers.push_back(pSampler);
				}
			}
		}
	}
}

void SimionApp::updateScene(State* s, Action* a)
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
	if ((pExperiment->getExperimentStep()-1) % m_functionViewUpdateStepFreq == 0)
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

void SimionApp::update2DMeters(State* s, Action* a)
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