#pragma once

#include "world.h"
#include "../../3rd-party/bullet3-2.86/src/btBulletDynamicsCommon.h"

class Robot;
class BulletBody;
class BulletPhysics;

//Move box with 2 robots
class RobotControl : public DynamicModel
{
	/// All-Simulation variables
	double MASS_TARGET;
	double MASS_ROBOT;
	double MASS_GROUND;

	/// State variables
	int m_target_X, m_target_Y;
	int m_rob1_X, m_rob1_Y;
	int m_omega;
	int m_theta;

	// Action variables
	int m_linear_vel;

	///inicialización
	BulletPhysics* m_pBulletPhysics;

public:
	RobotControl(ConfigNode* pParameters);
	virtual ~RobotControl();

	void reset(State *s);
	void executeAction(State *s, const Action *a, double dt);

};

