#pragma once

#include "world.h"
#include "../../../3rd-party/bullet3-2.86/src/btBulletDynamicsCommon.h"

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
	size_t m_target_X, m_target_Y;
	size_t m_rob1_X, m_rob1_Y;
	size_t m_omega;
	size_t m_theta;

	// Action variables
	size_t m_linear_vel;

	///inicialización
	BulletPhysics* m_pBulletPhysics;

public:
	RobotControl(ConfigNode* pParameters);
	virtual ~RobotControl();

	void reset(State *s);
	void executeAction(State *s, const Action *a, double dt);

};

