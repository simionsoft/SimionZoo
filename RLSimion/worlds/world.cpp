#include "world.h"
#include "../named-var-set.h"
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
#include "../SimGod.h"
#include "../logger.h"
#include "../experiment.h"

CHILD_OBJECT_FACTORY<DynamicModel> World::m_pDynamicModel;

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

void World::reset(State *s)
{
	m_episodeSimTime = 0.0;
	if (m_pDynamicModel.ptr())
		m_pDynamicModel->reset(s);
}

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



DynamicModel::~DynamicModel()
{
	if (m_pStateDescriptor) delete m_pStateDescriptor;
	if (m_pActionDescriptor) delete m_pActionDescriptor;
	if (m_pRewardFunction) delete m_pRewardFunction;
}



DynamicModel::DynamicModel()
{
	m_pStateDescriptor = new Descriptor();
	m_pActionDescriptor = new Descriptor();
	m_pRewardFunction = new RewardFunction();
}

size_t DynamicModel::addStateVariable(const char* name, const char* units, double min, double max, bool bCircular)
{
	return m_pStateDescriptor->addVariable(name, units, min, max, bCircular);
}

size_t DynamicModel::addActionVariable(const char* name, const char* units, double min, double max, bool bCircular)
{
	return m_pActionDescriptor->addVariable(name, units, min, max, bCircular);
}

void DynamicModel::addConstant(const char* name, double value)
{
	m_pConstants[name] = value;
}

int DynamicModel::getNumConstants()
{
	return (int)m_pConstants.size();
}

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

double DynamicModel::getReward(const State *s, const Action *a, const State *s_p)
{
	return m_pRewardFunction->getReward(s, a, s_p);
}

Reward* DynamicModel::getRewardVector()
{
	return m_pRewardFunction->getRewardVector();
}