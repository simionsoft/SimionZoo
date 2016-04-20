#include "stdafx.h"
#include "world.h"
#include "named-var-set.h"
#include "world-windturbine.h"
#include "world-underwatervehicle.h"
#include "world-pitchcontrol.h"
#include "world-magneticlevitation.h"
#include "reward.h"
#include "parameters.h"
#include "globals.h"
#include "app.h"
#include "simgod.h"

CDynamicModel* CWorld::m_pDynamicModel = 0;

CWorld::CWorld()
{
	m_pDynamicModel = 0;
	m_pRewardFunction = 0;
	m_numIntegrationSteps = 0;
	m_dt = 0.0;
	m_scalarReward = 0.0;
}

CLASS_INIT(CWorld)
{
	if (!pParameters) return;
	assert(pParameters);
	m_t= 0.0;

	//The dynamic model must be created before the reward so that references to state variables are correctly set
	CHILD_CLASS_FACTORY(m_pDynamicModel, "Dynamic-Model","The dynamic model",false, CDynamicModel);
	CHILD_CLASS(m_pRewardFunction, "Reward","The reward function",false, CRewardFunction);

	CONST_INTEGER_VALUE(m_numIntegrationSteps,"Num-Integration-Steps",4,"The number of integration steps performed each simulation time-step");
	CONST_DOUBLE_VALUE(m_dt,"Delta-T",0.01,"The delta-time between simulation steps");

	m_scalarReward = 0.0;
	END_CLASS();
}

CWorld::~CWorld()
{
	if (m_pDynamicModel)
	{
		delete m_pDynamicModel; m_pDynamicModel = 0;
	}
	if (m_pRewardFunction)
	{
		delete m_pRewardFunction; m_pRewardFunction = 0;
	}
}

double CWorld::getDT()
{
	return m_dt;
}

double CWorld::getT()
{
	return m_t;
}

double CWorld::getStepStartT()
{
	return m_step_start_t;
}

CReward* CWorld::getReward()
{
	return m_pRewardFunction->getReward();
}

void CWorld::reset(CState *s)
{
	m_scalarReward = 0.0;
	m_t= 0.0;
	if (m_pDynamicModel)
		m_pDynamicModel->reset(s);
}

double CWorld::executeAction(CState *s,CAction *a,CState *s_p)
{
	double dt= m_dt/m_numIntegrationSteps;

	m_step_start_t= m_t;

	if (m_pDynamicModel)
	{
		s_p->copy(s);
		for (int i= 0; i<m_numIntegrationSteps; i++)
		{
			m_pDynamicModel->executeAction(s_p,a,dt);
			m_t+= dt;
		}
	}
	m_scalarReward = m_pRewardFunction->calculateReward(s, a, s_p);

	return m_scalarReward;
}
//
//CState *CWorld::getStateDescriptor()
//{
//	if (m_pDynamicModel)
//		return m_pDynamicModel->getStateDescriptor();
//	return 0;
//}
//
//CAction *CWorld::getActionDescriptor()
//{
//	if (m_pDynamicModel)
//		return m_pDynamicModel->getActionDescriptor();
//	return 0;
//}
//


CDynamicModel::~CDynamicModel()
{ 
	if (m_pStateDescriptor) delete m_pStateDescriptor;
	if (m_pActionDescriptor) delete m_pActionDescriptor;
}


CDynamicModel::CDynamicModel(const char* pWorldDefinitionFile)
{
	tinyxml2::XMLDocument configXMLDoc;
	CParameters *rootNode;
	if (pWorldDefinitionFile)
	{
		CApp::SimGod.registerInputFile(pWorldDefinitionFile);
		configXMLDoc.LoadFile(pWorldDefinitionFile);
		if (!configXMLDoc.Error())
		{
			rootNode = (CParameters*)configXMLDoc.FirstChildElement("World-Definition");
			m_pStateDescriptor =new CState( rootNode->getChild("State"));
			m_pActionDescriptor = new CAction(rootNode->getChild("Action"));
			//we only copy the pointer because we are assuming the xml config document won't be deleted until the main program ends
			m_pConstants = rootNode->getChild("Constants");
		}
	}
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

CLASS_FACTORY(CDynamicModel)
{
	CHOICE_XML("Model", "WORLD-DEFINITION","The world");
	CHOICE_ELEMENT_XML("Wind-turbine", CWindTurbine, "../config/world/wind-turbine.xml","A two-mass model of a VS Wind Turbine");
	CHOICE_ELEMENT_XML("Underwater-vehicle", CUnderwaterVehicle, "../config/world/underwater-vehicle.xml","An underwater vehicle control task");
	CHOICE_ELEMENT_XML("Pitch-control", CPitchControl, "../config/world/pitch-control.xml","An airplane pitch control task");
	//CHOICE _ ELEMENT _ XML("Magnetic-leviation", CMagneticLevitation, "../config/world/magnetic-levitation.xml");
	END_CHOICE();
	return 0;

	END_CLASS();
}