#include "stdafx.h"
#include "world.h"
#include "named-var-set.h"
#include "world-windturbine.h"
#include "world-underwatervehicle.h"
#include "world-pitchcontrol.h"
#include "world-magneticlevitation.h"
#include "world-balancingpole.h"
#include "world-mountaincar.h"
#include "reward.h"
#include "config.h"
#include "app.h"
#include "SimGod.h"
#include "logger.h"

CHILD_OBJECT_FACTORY<CDynamicModel> CWorld::m_pDynamicModel;

CWorld::CWorld(CConfigNode* pConfigNode)
{
	if (!pConfigNode) return;
	m_t= 0.0;

	m_pDynamicModel=CHILD_OBJECT_FACTORY<CDynamicModel>(pConfigNode, "Dynamic-Model","The dynamic model");

	m_numIntegrationSteps= INT_PARAM(pConfigNode,"Num-Integration-Steps"
		,"The number of integration steps performed each simulation time-step",4);
	m_dt= DOUBLE_PARAM(pConfigNode,"Delta-T","The delta-time between simulation steps", 0.01);
}

CWorld::~CWorld()
{
}

double CWorld::getDT()
{
	return m_dt.get();
}

double CWorld::getT()
{
	return m_t;
}

double CWorld::getStepStartT()
{
	return m_step_start_t;
}

CReward* CWorld::getRewardVector()
{
	return m_pDynamicModel->getRewardVector();
}

void CWorld::reset(CState *s)
{
	m_t= 0.0;
	if (m_pDynamicModel.ptr())
		m_pDynamicModel->reset(s);
}

double CWorld::executeAction(CState *s,CAction *a,CState *s_p)
{
	double dt= m_dt/m_numIntegrationSteps;

	m_step_start_t= m_t;

	if (m_pDynamicModel.ptr())
	{
		s_p->copy(s);
		for (int i= 0; i<m_numIntegrationSteps.get(); i++)
		{
			m_pDynamicModel->executeAction(s_p,a,dt);
			m_t+= dt;
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
	m_pStateDescriptor = new CState();
	m_pActionDescriptor = new CAction();
}

int CDynamicModel::addStateVariable(const char* name, const char* units, double min, double max)
{
	return m_pStateDescriptor->addVariable(name, units, min, max);
}

int CDynamicModel::addActionVariable(const char* name, const char* units, double min, double max)
{
	return m_pActionDescriptor->addVariable(name, units, min, max);
}

void CDynamicModel::addConstant(const char* name, double value)
{
	m_pConstants[name] = value;
}

double CDynamicModel::getConstant(const char* constantName)
{
	return m_pConstants[constantName];
}

CState* CDynamicModel::getStateDescriptor()
{
	return m_pStateDescriptor;
}
CAction* CDynamicModel::getActionDescriptor()
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
	return CHOICE<CDynamicModel>(pConfigNode,"Model", "The world",
	CHOICE_ELEMENT_NEW(pConfigNode, CWindTurbine, "Wind-turbine","A two-mass model of a VS Wind Turbine","World=Wind-turbine"),
	CHOICE_ELEMENT_NEW(pConfigNode, CUnderwaterVehicle, "Underwater-vehicle", "An underwater vehicle control task","World=Underwater-vehicle"),
	CHOICE_ELEMENT_NEW(pConfigNode, CPitchControl, "Pitch-control", "An airplane pitch control task","World=Pitch-control"),
	CHOICE_ELEMENT_NEW(pConfigNode, CBalancingPole, "Balancing-pole", "The balancing pole control problem (Sutton)","World=Balancing-pole")
		);
	//CHOICE("Model","The world");
	//CHOICE_ELEMENT_XML("Wind-turbine", CWindTurbine, "../config/world/wind-turbine.xml","A two-mass model of a VS Wind Turbine");
	//CHOICE_ELEMENT_XML("Underwater-vehicle", CUnderwaterVehicle, "../config/world/underwater-vehicle.xml","An underwater vehicle control task");
	//CHOICE_ELEMENT_XML("Pitch-control", CPitchControl, "../config/world/pitch-control.xml","An airplane pitch control task");
	//CHOICE_ELEMENT_XML("Balancing-pole", CBalancingPole, "../config/world/balancing-pole.xml", "The balancing pole control problem (Sutton)");
	//CHOICE_ELEMENT_XML("Mountain-car", CMountainCar, "../config/world/mountain-car.xml", "The mountain-car task as implemented by Sutton")
	////CHOICE _ ELEMENT _ XML("Magnetic-leviation", CMagneticLevitation, "../config/world/magnetic-levitation.xml");
	//END_CHOICE();
	//return 0;
}

double CDynamicModel::getReward(const CState *s, const CAction *a, const CState *s_p)
{
	return m_pRewardFunction->getReward(s, a, s_p);
}

CReward* CDynamicModel::getRewardVector()
{
	return m_pRewardFunction->getRewardVector();
}