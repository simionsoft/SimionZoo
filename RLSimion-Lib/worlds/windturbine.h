#pragma once

#include "world.h"

class CSetPoint;

//ONE-MASS WIND TURBINE MODEL
class CWindTurbine: public CDynamicModel
{

	double m_initial_torque;
	double m_initial_blade_angle;

	CSetPoint **m_pTrainingWindData;
	CSetPoint* m_pEvaluationWindData;
	CSetPoint* m_pCurrentWindData;
	int m_numDataFiles;
	int m_currentDataFile;

	CSetPoint *m_pPowerSetpoint;

	double C_p(double lambda, double beta);
	double C_q(double lambda, double beta);
	double aerodynamicTorque(double tip_speed_ratio, double beta, double wind_speed);
	double aerodynamicPower(double tip_speed_ratio, double beta, double wind_speed);
	double aerodynamicPower(double cp, double wind_speed);
	void findSuitableParameters(double initial_wind_speed, double initial_rotor_speed
		, double &initial_torque, double &initial_blade_angle);

public:
	CWindTurbine(CConfigNode* pParameters);
	virtual ~CWindTurbine();

	void reset(CState *s);
	void executeAction(CState *s, const CAction *a,double dt);
};