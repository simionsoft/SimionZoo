#include "stdafx.h"
#include "world.h"
#include "parameters.h"
#include "states-and-actions.h"

#include "world-windturbine.h"
#include "world-underwatervehicle.h"
#include "world-pitchcontrol.h"
#include "world-magneticlevitation.h"
#include "reward.h"

double CWorld::m_t= 0.0;
double CWorld::m_dt= 0.0;
double CWorld::m_step_start_t= 0.0;

CWorld::CWorld(CParameters* pParameters)
{
	assert(pParameters);
	m_t= 0.0;

	m_simulationSteps= (int)pParameters->getDouble("WORLD/NUM_SIMULATION_STEPS");
	m_dt= pParameters->getDouble("WORLD/DELTA_T");

	if (strcmp(pParameters->getStringPtr("WORLD/DYNAMIC_MODEL"),"WIND_TURBINE_ONE_MASS")==0)
		m_pDynamicModel= new CWindTurbine(pParameters);
	else if (strcmp(pParameters->getStringPtr("WORLD/DYNAMIC_MODEL"),"UNDERWATER_VEHICLE")==0)
		m_pDynamicModel= new CUnderwaterVehicle(pParameters);
	else if (strcmp(pParameters->getStringPtr("WORLD/DYNAMIC_MODEL"),"PITCH_CONTROL")==0)
		m_pDynamicModel= new CPitchControl(pParameters);
	else if (strcmp(pParameters->getStringPtr("WORLD/DYNAMIC_MODEL"),"MAGNETIC_LEVITATION")==0)
		m_pDynamicModel= new CMagneticLevitation(pParameters);
	//else if (strcmp(pParameters->getStringPtr("WORLD/DYNAMIC_MODEL"),"HEATING_COIL")==0)
	//	m_pDynamicModel= new CHeatingCoil(pParameters);

	m_pReward = new CReward(pParameters);
}

CWorld::~CWorld()
{
	if (m_pDynamicModel) delete m_pDynamicModel;
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

void CWorld::reset(CState *s)
{
	m_avgReward = 0.0;
	m_lastReward = 0.0;
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
	m_lastReward = m_pReward->getReward(s, a, s_p);
	m_avgReward = (1.0 - m_avgRewardGain)*m_avgReward + m_avgRewardGain*m_lastReward;

	return m_lastReward;
}

double CWorld::getLastReward()
{
	return m_lastReward;
}

double CWorld::getAvgReward()
{
	return m_avgReward;

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