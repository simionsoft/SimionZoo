#include "stdafx.h"
#include "world.h"
#include "named-var-set.h"
#include "world-windturbine.h"
#include "world-underwatervehicle.h"
#include "world-pitchcontrol.h"
#include "world-magneticlevitation.h"
#include "world-balancingpole.h"
#include "reward.h"
#include "parameters.h"
#include "globals.h"
#include "app.h"
#include "simgod.h"
#include "logger.h"

CDynamicModel* CWorld::m_pDynamicModel = 0;

CWorld::CWorld()
{
	m_pDynamicModel = 0;
	m_numIntegrationSteps = 0;
	m_dt = 0.0;
}

CLASS_INIT(CWorld)
{
	if (!pParameters) return;
	assert(pParameters);
	m_t= 0.0;

	CHILD_CLASS_FACTORY(m_pDynamicModel, "Dynamic-Model","The dynamic model",false, CDynamicModel);

	CONST_INTEGER_VALUE(m_numIntegrationSteps,"Num-Integration-Steps",4,"The number of integration steps performed each simulation time-step");
	CONST_DOUBLE_VALUE(m_dt,"Delta-T",0.01,"The delta-time between simulation steps");

	END_CLASS();
}

CWorld::~CWorld()
{
	if (m_pDynamicModel)
	{
		delete m_pDynamicModel; m_pDynamicModel = 0;
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

CReward* CWorld::getRewardVector()
{
	return m_pDynamicModel->getRewardVector();
}

void CWorld::reset(CState *s)
{
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
	return m_pDynamicModel->getReward(s, a, s_p);
}



CDynamicModel::~CDynamicModel()
{
	if (m_pWorldConfigXMLDoc) delete m_pWorldConfigXMLDoc;
	if (m_pStateDescriptor) delete m_pStateDescriptor;
	if (m_pActionDescriptor) delete m_pActionDescriptor;
	if (m_pRewardFunction) delete m_pRewardFunction;
}


CDynamicModel::CDynamicModel(const char* pWorldDefinitionFile)
{
	m_pWorldConfigXMLDoc= new tinyxml2::XMLDocument();
	CParameters *rootNode;
	if (pWorldDefinitionFile)
	{
		CApp::get()->SimGod.registerInputFile(pWorldDefinitionFile);
		m_pWorldConfigXMLDoc->LoadFile(pWorldDefinitionFile);
		if (!m_pWorldConfigXMLDoc->Error())
		{
			m_pRewardFunction = new CRewardFunction();
			rootNode = (CParameters*)m_pWorldConfigXMLDoc->FirstChildElement("World-Definition");
			m_pStateDescriptor = new CState(rootNode->getChild("State"));
			m_pActionDescriptor = new CAction(rootNode->getChild("Action"));

			//we only copy the pointer because we are assuming the xml config document won't be deleted until the main program ends
			m_pConstants = rootNode->getChild("Constants");
		}
		else
			CLogger::logMessage(MessageType::Error, "Could not load the world definition file.");
	}
}

double CDynamicModel::getConstant(const char* constantName)
{
	CParameters* pNode = m_pConstants->getChild("Constant");
	CParameters* pChild;
	while (pNode)
	{
		pChild = pNode->getChild("Name");
		if (pChild && !strcmp(constantName, pChild->getConstString()))
			return pNode->getConstDouble("Value");

		pNode = pNode->getNextChild("Constant");
	}
	char message[1024];
	sprintf_s(message, 1024, "Missing constant in world definition file: %s", constantName);
	CApp::get()->Logger.logMessage(MessageType::Error, message);

	return -1.0;// will never reach this, but if this makes the compiler happy, so be it
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
	CHOICE_ELEMENT_XML("Balancing-pole", CBalancingPole, "../config/world/balancing-pole.xml", "The balancing pole control problem (Sutton)");
	//CHOICE _ ELEMENT _ XML("Magnetic-leviation", CMagneticLevitation, "../config/world/magnetic-levitation.xml");
	END_CHOICE();
	return 0;

	END_CLASS();
}

double CDynamicModel::getReward(const CState *s, const CAction *a, const CState *s_p)
{
	return m_pRewardFunction->getReward(s, a, s_p);
}

CReward* CDynamicModel::getRewardVector()
{
	return m_pRewardFunction->getRewardVector();
}