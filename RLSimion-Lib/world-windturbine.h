#pragma once

#include "world.h"

class CSetPoint;

//ONE-MASS WIND TURBINE MODEL
class CWindTurbine: public CDynamicModel
{
	//state handlers
	int m_sT_a, m_sP_a, m_sP_s, m_sP_e;
	int m_sE_p, m_sV, m_sOmega_r, m_sE_omega_r;
	int m_sD_omega_r, m_sBeta, m_sD_beta;
	int m_sT_g, m_sD_T_g, m_sE_int_omega_r;
	//action handlers
	int m_aD_beta, m_aD_T_g;

	double m_initial_torque;
	double m_initial_blade_angle;

	CSetPoint **m_pTrainingWindData;
	CSetPoint* m_pEvaluationWindData;
	CSetPoint* m_pCurrentWindData;
	int m_numDataFiles;
	int m_currentDataFile;

	CSetPoint *m_pPowerSetpoint;

public:
	CWindTurbine(CParameters* pParameters,const char* worldDefinition);
	~CWindTurbine();

	void reset(CState *s);
	void executeAction(CState *s,CAction *a,double dt);
};