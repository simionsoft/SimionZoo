#pragma once

#include "world.h"
#include "../../3rd-party/bullet3-2.86/src/btBulletDynamicsCommon.h"
class SimpleOpenGL3App;
class BulletPhysics;
class Robot;
class Box;
class BulletBody;
class Rope;


//Move box with 2 robots
class CPullBox2 : public CDynamicModel
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
	int m_rob2_X, m_rob2_Y;
	int m_box_X, m_box_Y;
	int m_D_Br1X, m_D_Br1Y;
	int m_D_Br2X, m_D_Br2Y;
	int m_D_BtX, m_D_BtY;
	int m_theta_r1;
	int m_theta_r2;
	int m_boxTheta;

	// Action variables
	int m_linear_vel_r1;
	int m_omega_r1;
	int m_linear_vel_r2;
	int m_omega_r2;

	///Graphic initialization
	
	BulletPhysics* m_pBulletPhysics;

	///Bullet bodies init
	BulletBody *m_pGround;
	BulletBody *m_pTarget;
	Robot *m_pRobot1;
	Robot *m_pRobot2;
	Box *m_pBox;
	Rope *m_Rope;
	


public:
	CPullBox2(CConfigNode* pParameters);
	virtual ~CPullBox2();

	void reset(CState *s);
	void executeAction(CState *s, const CAction *a, double dt);
};
