#pragma once

#include "world.h"
#include "../../3rd-party/bullet3-2.86/src/btBulletDynamicsCommon.h"

class Robot;
class BulletBody;
class BulletPhysics;

//Move box with 2 robots
class CRobotControl : public CDynamicModel
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
	CRobotControl(CConfigNode* pParameters);
	virtual ~CRobotControl();

	void reset(CState *s);
	void executeAction(CState *s, const CAction *a, double dt);

};

