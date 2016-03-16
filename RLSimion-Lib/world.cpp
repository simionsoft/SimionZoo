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

CDynamicModel* CWorld::m_pDynamicModel = 0;


CLASS_CONSTRUCTOR (CWorld) (CParameters* pParameters)
{
	assert(pParameters);
	m_t= 0.0;

	//The dynamic model must be created before the reward so that references to state variables are correctly set
	CHILD_CLASS_FACTORY(m_pDynamicModel, "Dynamic-Model", CDynamicModel, pParameters);
	CHILD_CLASS(m_pRewardFunction, "Reward", CRewardFunction, pParameters);

	CONST_INTEGER_VALUE(m_simulationSteps,pParameters,"Num-Integration-Steps",4);
	CONST_DOUBLE_VALUE(m_dt,pParameters,"Delta-T",0.01);

	m_scalarReward = 0.0;
	END_CLASS();
}

CWorld::~CWorld()
{
	if (m_pDynamicModel) delete m_pDynamicModel;
	if (m_pRewardFunction) delete m_pRewardFunction;
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
	double dt= m_dt/m_simulationSteps;

	m_step_start_t= m_t;

	if (m_pDynamicModel)
	{
		s_p->copy(s);
		for (int i= 0; i<m_simulationSteps; i++)
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
//CState *CWorld::getStateInstance()
//{
//	if (m_pDynamicModel)
//		return m_pDynamicModel->getStateDescriptor()->getInstance();
//	return 0;
//}
//
//CAction *CWorld::getActionInstance()
//{
//	if (m_pDynamicModel)
//		return m_pDynamicModel->getActionDescriptor()->getInstance();
//	return 0;
//}

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

CDynamicModel *CLASS_FACTORY(CDynamicModel)(CParameters* pParameters)
{
	CHOICE_XML("Model", "WORLD-DEFINITION");
	CHOICE_ELEMENT_XML("Wind-turbine", CWindTurbine, "../config/world/wind-turbine.xml");
	CHOICE_ELEMENT_XML("Underwater-vehicle", CUnderwaterVehicle, "../config/world/underwater-vehicle.xml");
	CHOICE_ELEMENT_XML("Pitch-control", CWindTurbine, "../config/world/pitch-control.xml");
	//CHOICE _ ELEMENT _ XML("Magnetic-leviation", CMagneticLevitation, "../config/world/magnetic-levitation.xml");
	END_CHOICE();
	return 0;

	END_CLASS();
}