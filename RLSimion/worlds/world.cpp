#include "world.h"
#include "../named-var-set.h"
#include "windturbine.h"
#include "underwatervehicle.h"
#include "pitchcontrol.h"
#include "magneticlevitation.h"
#include "balancingpole.h"
#include "swinguppendulum.h"
#include "gridworld.h"
#include "push-box-1.h"
#include "push-box-2.h"
#include "pull-box-1.h"
#include "pull-box-2.h"
#include "mountaincar.h"
#include "robot-control.h"
#include "FAST.h"
#include "../reward.h"
#include "../config.h"
#include "../app.h"
#include "../SimGod.h"
#include "../logger.h"

CHILD_OBJECT_FACTORY<CDynamicModel> CWorld::m_pDynamicModel;

CWorld::CWorld(CConfigNode* pConfigNode)
{
	if (!pConfigNode) return;
	m_episodeSimTime = 0.0;
	m_totalSimTime = 0.0;

	m_pDynamicModel = CHILD_OBJECT_FACTORY<CDynamicModel>(pConfigNode, "Dynamic-Model", "The dynamic model");

	m_numIntegrationSteps = INT_PARAM(pConfigNode, "Num-Integration-Steps"
		, "The number of integration steps performed each simulation time-step", 4);
	m_dt = DOUBLE_PARAM(pConfigNode, "Delta-T", "The delta-time between simulation steps", 0.01);
}

CWorld::~CWorld()
{
}

double CWorld::getDT()
{
	return m_dt.get();
}

double CWorld::getEpisodeSimTime()
{
	return m_episodeSimTime;
}

double CWorld::getTotalSimTime()
{
	return m_totalSimTime;
}

double CWorld::getStepStartSimTime()
{
	return m_stepStartSimTime;
}

CReward* CWorld::getRewardVector()
{
	return m_pDynamicModel->getRewardVector();
}

void CWorld::reset(CState *s)
{
	m_episodeSimTime = 0.0;
	if (m_pDynamicModel.ptr())
		m_pDynamicModel->reset(s);
}

double CWorld::executeAction(CState *s, CAction *a, CState *s_p)
{
	double dt = m_dt.get() / (double)m_numIntegrationSteps.get();

	m_stepStartSimTime = m_episodeSimTime;

	if (m_pDynamicModel.ptr())
	{
		s_p->copy(s);
		for (int i = 0; i < m_numIntegrationSteps.get() && CSimionApp::get()->pExperiment->isValidStep(); i++)
		{
			m_bFirstIntegrationStep = ( i==0 );
			m_pDynamicModel->executeAction(s_p, a, dt);
			m_episodeSimTime += dt;
			m_totalSimTime += dt;
		}
	}
	return m_pDynamicModel->getReward(s, a, s_p);
}



CDynamicModel::~CDynamicModel()
{
	if (m_pStateDescriptor) delete m_pStateDescriptor;
	if (m_pActionDescriptor) delete m_pActionDescriptor;
	if (m_pRewardFunction) delete m_pRewardFunction;
}


CDynamicModel::CDynamicModel()
{
	m_pStateDescriptor = new CDescriptor();
	m_pActionDescriptor = new CDescriptor();
	m_pRewardFunction = new CRewardFunction();
}

int CDynamicModel::addStateVariable(const char* name, const char* units, double min, double max, bool bCircular)
{
	return m_pStateDescriptor->addVariable(name, units, min, max, bCircular);
}

int CDynamicModel::addActionVariable(const char* name, const char* units, double min, double max, bool bCircular)
{
	return m_pActionDescriptor->addVariable(name, units, min, max, bCircular);
}

void CDynamicModel::addConstant(const char* name, double value)
{
	m_pConstants[name] = value;
}

int CDynamicModel::getNumConstants()
{
	return (int)m_pConstants.size();
}

double CDynamicModel::getConstant(int i)
{
	int j = 0;
	for (std::map<const char*, double>::iterator it = m_pConstants.begin(); it != m_pConstants.end(); ++it)
	{
		if (j == i) return it->second;

		++j;
	}
	return 0.0;
}

const char* CDynamicModel::getConstantName(int i)
{
	int j = 0;
	for (std::map<const char*, double>::iterator it = m_pConstants.begin(); it != m_pConstants.end(); ++it)
	{
		if (j == i) return it->first;

		++j;
	}
	return "";
}

double CDynamicModel::getConstant(const char* constantName)
{
	if (m_pConstants.find(constantName) != m_pConstants.end())
		return m_pConstants[constantName];
	CLogger::logMessage(MessageType::Error
		, (std::string("CDynamicModel::getConstant() couldn't find constant: ") + std::string(constantName)).c_str());
	return 0.0;
}

CDescriptor& CDynamicModel::getStateDescriptor()
{
	return *m_pStateDescriptor;
}
CDescriptor& CDynamicModel::getActionDescriptor()
{
	return *m_pActionDescriptor;
}
CDescriptor* CDynamicModel::getStateDescriptorPtr()
{
	return m_pStateDescriptor;
}
CDescriptor* CDynamicModel::getActionDescriptorPtr()
{
	return m_pActionDescriptor;
}
CState* CDynamicModel::getStateInstance()
{
	return m_pStateDescriptor->getInstance();
}
CAction* CDynamicModel::getActionInstance()
{
	return m_pActionDescriptor->getInstance();
}

std::shared_ptr<CDynamicModel> CDynamicModel::getInstance(CConfigNode* pConfigNode)
{
	return CHOICE<CDynamicModel>(pConfigNode, "Model", "The world",
	{
		{make_tuple("Wind-turbine",CHOICE_ELEMENT_NEW<CWindTurbine>,"World=Wind-turbine")},
		{make_tuple("FAST-Wind-turbine",CHOICE_ELEMENT_NEW<CFASTWindTurbine>,"World=FAST-Wind-turbine") },
		{make_tuple("Underwater-vehicle", CHOICE_ELEMENT_NEW<CUnderwaterVehicle>,"World=Underwater-vehicle")},
		{make_tuple("Pitch-control",CHOICE_ELEMENT_NEW<CPitchControl>,"World=Pitch-control")},
		{make_tuple("Balancing-pole",CHOICE_ELEMENT_NEW<CBalancingPole>,"World=Balancing-pole")},
		{make_tuple("Push-Box-2",CHOICE_ELEMENT_NEW<CPushBox2>,"World=Push-Box-2")},
		{make_tuple("Push-Box-1",CHOICE_ELEMENT_NEW<CPushBox1>,"World=Push-Box-1")},
		{make_tuple("Robot-control",CHOICE_ELEMENT_NEW<CRobotControl>,"World=Robot-control") },
		{make_tuple("Pull-Box-2",CHOICE_ELEMENT_NEW<CPullBox2>,"World=Pull-Box-2") },
		{make_tuple("Pull-Box-1",CHOICE_ELEMENT_NEW<CPullBox1>,"World=Pull-Box-1") },
		{ make_tuple("Mountain-car",CHOICE_ELEMENT_NEW<CMountainCar>,"World=Mountain-car") },
		{ make_tuple("Swing-up-pendulum",CHOICE_ELEMENT_NEW<CSwingupPendulum>,"World=Swing-up-pendulum") },
		{ make_tuple("Continuous-Gridworld",CHOICE_ELEMENT_NEW<CContinuousGridWorld>,"World=Continuous-GridWorld") },
		{ make_tuple("Discrete-Gridworld",CHOICE_ELEMENT_NEW<CDiscreteGridWorld>,"World=Discrete-GridWorld") }
	});
}

double CDynamicModel::getReward(const CState *s, const CAction *a, const CState *s_p)
{
	return m_pRewardFunction->getReward(s, a, s_p);
}

CReward* CDynamicModel::getRewardVector()
{
	return m_pRewardFunction->getRewardVector();
}