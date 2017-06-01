#pragma once

#include "world.h"
#define ATTRIBUTE_ALIGNED16(a)
#include "../3rd-party/bullet3-2.86/src/btBulletDynamicsCommon.h"
class SimpleOpenGL3App;
class BulletPhysics;
class BulletGraphic;
class Robot;
class Box;
class BulletBody;
class Rope;


//Move box with 1 robot and 1 rope
class CRopeRobot : public CDynamicModel
{
	/// All-Simulation variables
	double MASS_ROBOT;
	double MASS_BOX;
	double MASS_TARGET;
	double MASS_GROUND;

	/// State variables
	int m_target_X, m_target_Y;
	int m_state_X, m_state_Y;
	int m_rob1_X, m_rob1_Y;
	int m_box_X, m_box_Y;
	int m_D_Br1X, m_D_Br1Y;;
	int m_D_BtX, m_D_BtY;
	int m_theta_r1;
	int m_boxTheta;

	// Action variables
	int m_linear_vel_r1;
	int m_omega_r1;

	///Graphic initialization
	BulletPhysics* robRopeBuilder;
	BulletGraphic* robRopeViewer;

	///Bullet bodies init
	BulletBody *m_Ground;
	BulletBody *m_Target;
	Robot *m_Robot1;
	Box *m_Box;
	Rope *m_Rope;


public:
	CRopeRobot(CConfigNode* pParameters);
	virtual ~CRopeRobot();

	void reset(CState *s);
	void executeAction(CState *s, const CAction *a, double dt);
};
