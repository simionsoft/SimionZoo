#pragma once

#include "world.h"
#include "../utils.h"

class SetPoint;

//ONE-MASS WIND TURBINE MODEL
class WindTurbine: public DynamicModel
{
	SetPoint **m_pTrainingWindData;
	SetPoint* m_pEvaluationWindData;
	SetPoint* m_pCurrentWindData;
	size_t m_numDataFiles;
	size_t m_currentDataFile;

	SetPoint *m_pPowerSetpoint;
	Table m_Cp;

	double C_p(double lambda, double beta);
	double C_q(double lambda, double beta);
	double aerodynamicTorque(double tip_speed_ratio, double beta, double wind_speed);
	double aerodynamicPower(double tip_speed_ratio, double beta, double wind_speed);
	double aerodynamicPower(double cp, double wind_speed);
	void findSuitableParameters(double initial_wind_speed, double& initial_rotor_speed, double &initial_blade_angle);

public:
	WindTurbine(ConfigNode* pParameters);
	virtual ~WindTurbine();

	void reset(State *s);
	void executeAction(State *s, const Action *a,double dt);
};