/*
	SimionZoo: A framework for online model-free Reinforcement Learning on continuous
	control problems

	Copyright (c) 2016 SimionSoft. https://github.com/simionsoft

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#include "app.h"
#include "logger.h"
#include "worlds/world.h"
#include "experiment.h"
#include "simgod.h"
#include "config.h"
#include "utils.h"
#include "function-sampler.h"
#include "../Common/state-action-function.h"
#include "../Common/wire.h"
#include "../../tools/OpenGLRenderer/basic-shapes-2d.h"
#include "../../tools/OpenGLRenderer/renderer.h"
#include "../../tools/OpenGLRenderer/text.h"
#include "../../tools/OpenGLRenderer/input-handler.h"
#include "../../tools/OpenGLRenderer/material.h"
#include "../../tools/OpenGLRenderer/arranger.h"
#include "../../tools/System/FileUtils.h"
#include "../../tools/System/CrossPlatform.h"

#include <algorithm>

//Properties and xml tags
#define APP_REQUIREMENTS_XML_TAG "Requirements"
#define TARGET_PLATFORM_REQUIREMENTS_XML_TAG "Target-Platform"
#define TARGET_PLATFORM_NAME_ATTR_TAG "Name"
#define NUM_CPU_CORES_XML_TAG "NumCPUCores"

SimionApp* SimionApp::m_pAppInstance = 0;

SimionApp::SimionApp(ConfigNode* pConfigNode)
{
	m_pAppInstance = this;

	pConfigNode = pConfigNode->getChild("RLSimion");
	if (!pConfigNode) throw std::runtime_error("Wrong experiment configuration file");

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

	for (FunctionSampler* sampler : m_pFunctionSamplers) delete sampler;
	for (pair<string, Wire*> p : m_wires) delete p.second;

	m_pAppInstance = 0;

	//delete target platform-specific requirements
	for (auto it = m_targetPlatformRequirements.begin(); it != m_targetPlatformRequirements.end(); it++)
	{
		delete (*it).second;
	}
}

const char* SimionApp::getArgValue(int argc,char** argv,const char* argName)
{
	char argPrefix[256];
	CrossPlatform::Sprintf_s(argPrefix, 256, "-%s=", argName);
	for (int i = 1; i < argc; ++i)
	{
		if (strstr(argv[i], argPrefix) == argv[i])
		{
			return (char*)(argv[i]) + strlen(argPrefix);
		}
	}
	return nullptr;
}

bool SimionApp::flagPassed(int argc, char** argv, const char* flagName)
{
	char argPrefix[256];
	CrossPlatform::Sprintf_s(argPrefix, 256, "-%s", flagName);
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
	printf("<%s>\n", APP_REQUIREMENTS_XML_TAG);
	m_commonRequirements.printXML();

	//other requirements: #CPU cores
	printf("  <%s>%d</%s>\n", NUM_CPU_CORES_XML_TAG, m_numCPUCores, NUM_CPU_CORES_XML_TAG);
	
	//target-platform requirements: architecture
	for (auto it = m_targetPlatformRequirements.begin(); it != m_targetPlatformRequirements.end(); it++)
	{
		printf("  <%s %s=\"%s\">\n", TARGET_PLATFORM_REQUIREMENTS_XML_TAG, TARGET_PLATFORM_NAME_ATTR_TAG, (*it).first.c_str());
		(*it).second->printXML();
		printf("  </%s>\n", TARGET_PLATFORM_REQUIREMENTS_XML_TAG);
	}

	printf("</%s>\n", APP_REQUIREMENTS_XML_TAG);
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
	string finalName = name;
	size_t index = 0;
	while (m_pStateActionFunctions.find(finalName) != m_pStateActionFunctions.end())
	{
		finalName = name + "_" + to_string(index);
		index++;
	}
	m_pStateActionFunctions[finalName] = pFunction;
}

void SimionApp::wireRegister(string name)
{
	if (m_wires.find(name) == m_wires.end())
		m_wires[name] = new Wire(name);
}

void SimionApp::wireRegister(string name, double minimum, double maximum)
{
	auto wire = m_wires.find(name);
	if (wire == m_wires.end())
		m_wires[name] = new Wire(name, minimum, maximum);
	else
	{
		//we set the limits because they might not have any limits in another instance of the Wire
		m_wires[name]->setLimits(minimum, maximum);
	}
}

Wire* SimionApp::wireGet(string name)
{
	auto wire = m_wires.find(name);
	if (wire != m_wires.end())
		return wire->second;
	return nullptr;
}

void SimionApp::addPlatform(string targetPlatform)
{
	if (m_targetPlatformRequirements.find(targetPlatform) == m_targetPlatformRequirements.end())
	{
		m_targetPlatformRequirements[targetPlatform] = new RunTimeRequirements();
	}
}

void SimionApp::registerTargetPlatformInputFile(const char* targetPlatform, const char* filepath, const char* rename)
{
	addPlatform(string(targetPlatform));
	m_targetPlatformRequirements[targetPlatform]->addInputFile(filepath, rename);
}

void SimionApp::registerTargetPlatformOutputFile(const char* targetPlatform, const char* filepath)
{
	addPlatform(string(targetPlatform));
	m_targetPlatformRequirements[targetPlatform]->addOutputFile(filepath);
}

void SimionApp::registerInputFile(const char* filepath, const char* rename)
{
	m_commonRequirements.addInputFile(filepath, rename);
}

void SimionApp::registerOutputFile(const char* filepath)
{
	m_commonRequirements.addOutputFile(filepath);
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
		//scene file names are in lower case
		std::transform(sceneFile.begin(), sceneFile.end(), sceneFile.begin(), ::tolower);
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
		for (FunctionSampler* pSampler : m_pFunctionSamplers)
		{
			//create the function viewer
			FunctionViewer* pFunctionViewer = nullptr;
			//depending on the dimensions, we create a 3d viewer or a 2d viewer
			if (pSampler->getNumSamplesY() > 1)
				pFunctionViewer= new FunctionViewer3D(pSampler->getFunctionId(), Vector2D(0.0, 0.0), Vector2D(0.0, 0.0), m_numSamplesPerDim, 0.25);
			else if (pSampler->getNumSamplesY() == 1)
				pFunctionViewer = new FunctionViewer2D(pSampler->getFunctionId(), Vector2D(0.0, 0.0), Vector2D(0.0, 0.0), m_numSamplesPerDim, 0.25);

			if (pFunctionViewer)
			{
				m_pFunctionViews[pFunctionViewer] = pSampler;

				m_pRenderer->add2DGraphicObject(pFunctionViewer, functionViewPort);
				objects.push_back(pFunctionViewer);
			}
		}

		Arranger objectArranger;
		objectArranger.arrange2DObjects(objects, Vector2D(0.0,0.0), Vector2D(1.0,1.0), Vector2D(0.15,0.25)
			, Vector2D(0.20, 1.0), Vector2D(0.0075, 0.06));

		objectArranger.tag2DObjects(objects, functionViewPort);
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
		if (numInputs > 0)
		{
			//store all sampled variables to group them in pairs
			m_sampledVariables.clear();
			for (string varName : functionIt.second->getInputStateVariables())
				m_sampledVariables.push_back(make_pair(VariableSource::StateSource, varName));
			for (string varName : functionIt.second->getInputActionVariables())
				m_sampledVariables.push_back(make_pair(VariableSource::ActionSource, varName));

			for (size_t outputIndex = 0; outputIndex < functionIt.second->getNumOutputs(); outputIndex++)
			{
				//3d sampler
				if (numInputs > 1)
				{
					//create the 3D sampler: only the two first inputs will be used
					FunctionSampler* pSampler = new FunctionSampler3D(functionIt.first, functionIt.second, outputIndex, m_numSamplesPerDim
						, s->getDescriptor(), a->getDescriptor(), m_sampledVariables[0].first, m_sampledVariables[0].second
						, m_sampledVariables[1].first, m_sampledVariables[1].second);
					m_pFunctionSamplers.push_back(pSampler);
				}
				//2d sampler
				else if (numInputs == 1)
				{
					//create the 2D sampler: only one input
					FunctionSampler* pSampler = new FunctionSampler2D(functionIt.first, functionIt.second, outputIndex, m_numSamplesPerDim
						, s->getDescriptor(), a->getDescriptor(), m_sampledVariables[0].first, m_sampledVariables[0].second);
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
		for (auto functionIt : m_pFunctionViews)
		{
			const vector<double>& sampledValues = functionIt.second->sample();
			functionIt.first->update(sampledValues);
		}
	}
	//render the image
	m_pInputHandler->handleInput();
	m_pRenderer->draw();
}

void SimionApp::update2DMeters(State* s, Action* a)
{
	//2D METERS
	//update stats
	unsigned int statIndex = 0;
	for (Meter2D* pMeter2D : m_pStatsUIMeters)
	{
		IStats* pStat = pLogger->getStats(statIndex);
		pMeter2D->setValue(pStat->get());
		pMeter2D->setValueRange(Range(pStat->getStatsInfo()->getMin(), pStat->getStatsInfo()->getMax()));
		++statIndex;
	}
	//update state
	for (int i = 0; i < (int) s->getNumVars(); i++)
		m_pStateUIMeters[i]->setValue(s->get(i));

	//update action
	for (int i = 0; i < (int) a->getNumVars(); i++)
		m_pActionUIMeters[i]->setValue(a->get(i));
}

#include "CNTKWrapperClient.h"
void SimionApp::setPreferredDevice(Device device)
{
	//need to check whether CNTK has been loaded. Might not be used
	if (CNTK::WrapperClient::setDevice != nullptr)
	{
		if (device==CPU)
			Logger::logMessage(MessageType::Info, "Setting CPU as default device");
		else
			Logger::logMessage(MessageType::Info, "Setting GPU as default device");
		CNTK::WrapperClient::setDevice(device == Device::GPU);
	}
}