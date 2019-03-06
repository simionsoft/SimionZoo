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

#include "world.h"
#include "../../Common/named-var-set.h"
#include "windturbine.h"
#include "underwatervehicle.h"
#include "pitchcontrol.h"
#include "balancingpole.h"
#include "swinguppendulum.h"
#include "push-box-1.h"
#include "push-box-2.h"
#include "pull-box-1.h"
#include "pull-box-2.h"
#include "mountaincar.h"
#include "robot-control.h"
#include "double-pendulum.h"
#include "raincar.h"
#include "FAST.h"
#include "../reward.h"
#include "../config.h"
#include "../app.h"
#include "../simgod.h"
#include "../logger.h"
#include "../experiment.h"

CHILD_OBJECT_FACTORY<DynamicModel> World::m_pDynamicModel;


/// <summary>
/// Common constructor of the base class called before the subclass constructor
/// </summary>
/// <param name="pConfigNode"></param>
World::World(ConfigNode* pConfigNode)
{
	if (!pConfigNode) return;
	m_episodeSimTime = 0.0;
	m_totalSimTime = 0.0;

	m_pDynamicModel = CHILD_OBJECT_FACTORY<DynamicModel>(pConfigNode, "Dynamic-Model", "The dynamic model");

	m_numIntegrationSteps = INT_PARAM(pConfigNode, "Num-Integration-Steps"
		, "The number of integration steps performed each simulation time-step", 4);
	m_dt = DOUBLE_PARAM(pConfigNode, "Delta-T", "The delta-time between simulation steps", 0.01);
}

World::~World()
{
}


/// <summary>
/// This method returns the Delta_t used in the experiment
/// </summary>
/// <returns>The Delta_t used in the experiment</returns>
double World::getDT()
{
	return m_dt.get();
}

double World::getEpisodeSimTime()
{
	return m_episodeSimTime;
}

double World::getTotalSimTime()
{
	return m_totalSimTime;
}

double World::getStepStartSimTime()
{
	return m_stepStartSimTime;
}

Reward* World::getRewardVector()
{
	return m_pDynamicModel->getRewardVector();
}

/// <summary>
/// Reset state variables to the initial state from which simulations begin (it may be random)
/// </summary>
/// <param name="s">State variable that holds the initial state</param>
void World::reset(State *s)
{
	m_episodeSimTime = 0.0;
	if (m_pDynamicModel.ptr())
		m_pDynamicModel->reset(s);
}


/// <summary>
/// Method called every control time-step. Internally it calculates calculates the length
/// of the integration steps and calls several times DynamicModel::executeAction()
/// </summary>
/// <param name="s">The variable with the current state values</param>
/// <param name="a">The action to be executed</param>
/// <param name="s_p">The variable that will hold the resultant state</param>
/// <returns>The reward obtained in this step</returns>
double World::executeAction(State *s, Action *a, State *s_p)
{
	double dt = m_dt.get() / (double)m_numIntegrationSteps.get();

	m_stepStartSimTime = m_episodeSimTime;

	if (m_pDynamicModel.ptr())
	{
		s_p->copy(s);
		for (int i = 0; i < m_numIntegrationSteps.get() && SimionApp::get()->pExperiment->isValidStep(); i++)
		{
			m_bFirstIntegrationStep = ( i==0 );
			m_pDynamicModel->executeAction(s_p, a, dt);
			m_episodeSimTime += dt;
			m_totalSimTime += dt;
		}
	}
	return m_pDynamicModel->getReward(s, a, s_p);
}


/// <summary>
/// Base-class destructor
/// </summary>
DynamicModel::~DynamicModel()
{
	if (m_pStateDescriptor) delete m_pStateDescriptor;
	if (m_pActionDescriptor) delete m_pActionDescriptor;
	if (m_pRewardFunction) delete m_pRewardFunction;
}


/// <summary>
/// Base-class constructor
/// </summary>
DynamicModel::DynamicModel()
{
	SimionApp* pSimionApp = SimionApp::get();

	m_pStateDescriptor = new Descriptor(pSimionApp);	//the simion app is the default wire manager
	m_pActionDescriptor = new Descriptor(pSimionApp);

	m_pRewardFunction = new RewardFunction();
}

/// <summary>
/// This method must be called from the constructor of DynamicModel subclasses to register state variables. Calls are parsed
/// by the source code parser and listed beside the parameters of the class in the class definition file (config.xml)
/// </summary>
/// <param name="name">Name of the variable (i.e. "speed")</param>
/// <param name="units">Metrical unit (i.e., "m/s")</param>
/// <param name="min">Minimum value this variable may get. Below this, values are clamped</param>
/// <param name="max">Maximum value this variable may get. Above this, values are clamped</param>
/// <param name="bCircular">This flag indicates whether the variable is circular (as angles)</param>
/// <returns>The index of the state variable. This index may be used instead of the name for faster access</returns>
size_t DynamicModel::addStateVariable(const char* name, const char* units, double min, double max, bool bCircular)
{
	return m_pStateDescriptor->addVariable(name, units, min, max, bCircular);
}

/// <summary>
/// This method must be called from the constructor of DynamicModel subclasses to register action variables. Calls are
/// parsed by the source code parser and listed beside the parameters of the class in the class definition file (config.xml)
/// </summary>
/// <param name="name">Name of the variable (i.e. "speed")</param>
/// <param name="units">Metrical unit (i.e., "m/s")</param>
/// <param name="min">Minimum value this variable may get. Below this, values are clamped</param>
/// <param name="max">Maximum value this variable may get. Above this, values are clamped</param>
/// <param name="bCircular">This flag indicates whether the variable is circular (as angles)</param>
/// <returns>The index of the action variable. This index may be used instead of the name for faster access</returns>
size_t DynamicModel::addActionVariable(const char* name, const char* units, double min, double max, bool bCircular)
{
	return m_pActionDescriptor->addVariable(name, units, min, max, bCircular);
}

/// <summary>
/// This method can be called from the constructor of DynamicModel subclasses to register constants. These are also parsed
/// </summary>
/// <param name="name">Name of the constant</param>
/// <param name="value">Literal value (i.e. 6.5). The parser will not recognise but literal values</param>
void DynamicModel::addConstant(const char* name, double value)
{
	m_pConstants[name] = value;
}

/// <summary>
/// Returns the number of constants defined in the current DynamicModel subclass
/// </summary>
int DynamicModel::getNumConstants()
{
	return (int)m_pConstants.size();
}


/// <summary>
/// Returns the value of the i-th constant
/// </summary>
/// <param name="i">Index of the constant to be retrieved</param>
/// <returns>The value of the constant</returns>
double DynamicModel::getConstant(int i)
{
	int j = 0;
	for (std::map<const char*, double>::iterator it = m_pConstants.begin(); it != m_pConstants.end(); ++it)
	{
		if (j == i) return it->second;

		++j;
	}
	return 0.0;
}

/// <summary>
/// This method returns a constant's name
/// </summary>
/// <param name="i">Index of the constant</param>
/// <returns>The constant's name</returns>
const char* DynamicModel::getConstantName(int i)
{
	int j = 0;
	for (std::map<const char*, double>::iterator it = m_pConstants.begin(); it != m_pConstants.end(); ++it)
	{
		if (j == i) return it->first;

		++j;
	}
	return "";
}


/// <summary>
/// An alternative version of getConstant() that uses the name as input
/// </summary>
/// <param name="constantName">The name of the constant</param>
/// <returns>Its value</returns>
double DynamicModel::getConstant(const char* constantName)
{
	if (m_pConstants.find(constantName) != m_pConstants.end())
		return m_pConstants[constantName];
	Logger::logMessage(MessageType::Error
		, (std::string("DynamicModel::getConstant() couldn't find constant: ") + std::string(constantName)).c_str());
	return 0.0;
}

Descriptor& DynamicModel::getStateDescriptor()
{
	return *m_pStateDescriptor;
}
Descriptor& DynamicModel::getActionDescriptor()
{
	return *m_pActionDescriptor;
}
Descriptor* DynamicModel::getStateDescriptorPtr()
{
	return m_pStateDescriptor;
}
Descriptor* DynamicModel::getActionDescriptorPtr()
{
	return m_pActionDescriptor;
}
State* DynamicModel::getStateInstance()
{
	return m_pStateDescriptor->getInstance();
}
Action* DynamicModel::getActionInstance()
{
	return m_pActionDescriptor->getInstance();
}

std::shared_ptr<DynamicModel> DynamicModel::getInstance(ConfigNode* pConfigNode)
{
	return CHOICE<DynamicModel>(pConfigNode, "Model", "The world",
	{
		{make_tuple("Wind-turbine",CHOICE_ELEMENT_NEW<WindTurbine>,"World=Wind-turbine")},
		{make_tuple("FAST-Wind-turbine",CHOICE_ELEMENT_NEW<FASTWindTurbine>,"World=FAST-Wind-turbine") },
		{make_tuple("Underwater-vehicle", CHOICE_ELEMENT_NEW<UnderwaterVehicle>,"World=Underwater-vehicle")},
		{make_tuple("Pitch-control",CHOICE_ELEMENT_NEW<PitchControl>,"World=Pitch-control")},
		{make_tuple("Balancing-pole",CHOICE_ELEMENT_NEW<BalancingPole>,"World=Balancing-pole")},
		{make_tuple("Push-Box-2",CHOICE_ELEMENT_NEW<PushBox2>,"World=Push-Box-2")},
		{make_tuple("Push-Box-1",CHOICE_ELEMENT_NEW<PushBox1>,"World=Push-Box-1")},
		{make_tuple("Robot-control",CHOICE_ELEMENT_NEW<RobotControl>,"World=Robot-control") },
		{make_tuple("Pull-Box-2",CHOICE_ELEMENT_NEW<PullBox2>,"World=Pull-Box-2") },
		{make_tuple("Pull-Box-1",CHOICE_ELEMENT_NEW<PullBox1>,"World=Pull-Box-1") },
		{make_tuple("Mountain-car",CHOICE_ELEMENT_NEW<MountainCar>,"World=Mountain-car") },
		{make_tuple("Swing-up-pendulum",CHOICE_ELEMENT_NEW<SwingupPendulum>,"World=Swing-up-pendulum") },
		{make_tuple("Double-pendulum",CHOICE_ELEMENT_NEW<DoublePendulum>,"World=Double-pendulum") },
		{make_tuple("Rain-car",CHOICE_ELEMENT_NEW<RainCar>,"World=Rain-car") }
	});
}


/// <summary>
/// This method calculates the reward associated with tuple {s,a,s_p}
/// </summary>
/// <param name="s">Initial state</param>
/// <param name="a">Action</param>
/// <param name="s_p">Resultant state</param>
/// <returns>The reward</returns>
double DynamicModel::getReward(const State *s, const Action *a, const State *s_p)
{
	return m_pRewardFunction->getReward(s, a, s_p);
}

Reward* DynamicModel::getRewardVector()
{
	return m_pRewardFunction->getRewardVector();
}