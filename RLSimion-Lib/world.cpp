#include "stdafx.h"
#include "world.h"

#include "named-var-set.h"

#include "world-windturbine.h"
#include "world-underwatervehicle.h"
#include "world-pitchcontrol.h"
#include "world-magneticlevitation.h"
#include "reward.h"
#include "xml-parameters.h"

CDynamicModel* CWorld::m_pDynamicModel = 0;


CWorld::CWorld(tinyxml2::XMLElement* pParameters)
{
	assert(pParameters);
	m_t= 0.0;

	m_simulationSteps= XMLUtils::getConstInteger(pParameters,"Num-Integration-Steps");
	m_dt = XMLUtils::getConstDouble(pParameters, "Delta-T");

	tinyxml2::XMLElement* pModelParameters;
	pModelParameters = pParameters->FirstChildElement("Dynamic-Model")->FirstChildElement();

	if (strcmp(pModelParameters->Name(), "Wind-turbine") == 0)
		m_pDynamicModel = new CWindTurbine(pModelParameters);
	else if (strcmp(pModelParameters->Name(), "Underwater-vehicle") == 0)
		m_pDynamicModel = new CUnderwaterVehicle(pModelParameters);
	else if (strcmp(pModelParameters->Name(), "Pitch-control") == 0)
		m_pDynamicModel = new CPitchControl(pModelParameters);
	else if (strcmp(pModelParameters->Name(), "Magnetic-leviation") == 0)
		m_pDynamicModel = new CMagneticLevitation(pModelParameters);
	else m_pDynamicModel = 0;
	//else if (strcmp(pParameters->getStringPtr("DYNAMIC_MODEL"),"HEATING_COIL")==0)
	//	m_pDynamicModel= new CHeatingCoil(pParameters);

	m_pRewardFunction = new CRewardFunction(pParameters->FirstChildElement("Reward"));
	m_scalarReward = 0.0;
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

CState *CWorld::getStateDescriptor()
{
	if (m_pDynamicModel)
		return m_pDynamicModel->getStateDescriptor();
	return 0;
}

CAction *CWorld::getActionDescriptor()
{
	if (m_pDynamicModel)
		return m_pDynamicModel->getActionDescriptor();
	return 0;
}

CState *CWorld::getStateInstance()
{
	if (m_pDynamicModel)
		return m_pDynamicModel->getStateDescriptor()->getInstance();
	return 0;
}

CAction *CWorld::getActionInstance()
{
	if (m_pDynamicModel)
		return m_pDynamicModel->getActionDescriptor()->getInstance();
	return 0;
}

CDynamicModel::~CDynamicModel()
{ 
	if (m_pStateDescriptor) delete m_pStateDescriptor;
	if (m_pActionDescriptor) delete m_pActionDescriptor;
}


CDynamicModel::CDynamicModel(const char* pWorldDefinitionFile)
{
	tinyxml2::XMLDocument configXMLDoc;
	tinyxml2::XMLElement *rootNode;
	if (pWorldDefinitionFile)
	{
		
		configXMLDoc.LoadFile(pWorldDefinitionFile);
		if (!configXMLDoc.Error())
		{
			rootNode = configXMLDoc.FirstChildElement("World-Definition");
			m_pStateDescriptor =new CState( rootNode->FirstChildElement("State"));
			m_pActionDescriptor = new CAction(rootNode->FirstChildElement("Action"));
			//we only copy the pointer because we are assuming the xml config document won't be deleted until the main program ends
			m_pConstants = rootNode->FirstChildElement("Constants");
		}
	}
}