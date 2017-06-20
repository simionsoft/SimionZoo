#pragma once

#include "world.h"
#include "../../3rd-party/bullet3-2.86/src/btBulletDynamicsCommon.h"
class SimpleOpenGL3App;
class BulletPhysics;
class BulletBody;
class Box;
class Robot;

//Move box with 1 robot
class CPushBox1 : public CDynamicModel
{
	/// All-Simulation variables
	double MASS_ROBOT;
	double MASS_BOX;
	double MASS_TARGET;
	double MASS_GROUND;

	/// State variables
	int m_target_X, m_target_Y;
	int m_rob1_X, m_rob1_Y;
	int m_box_X, m_box_Y;
	int m_D_BrX, m_D_BrY;
	int m_D_BtX, m_D_BtY;
	int m_theta;
	int m_boxTheta;

	// Action variables
	int m_linear_vel;
	int m_omega;

	///Graphic initialization
	BulletPhysics* m_pBulletPhysics;

	///Bullet bodies init
	BulletBody *m_pGround= 0;
	Robot *m_Robot= 0;
	Box *m_pBox= 0;
	BulletBody *m_pTarget= 0;
	BulletBody *m_pWall1= 0, *m_pWall2= 0, *m_pWall3= 0, *m_pWall4= 0;

public:

	CPushBox1(CConfigNode* pParameters);
	virtual ~CPushBox1();

	void reset(CState *s);
	void executeAction(CState *s, const CAction *a, double dt);

};