#include "../stdafx.h"
#include "../app.h"
#include "../noise.h"

#include "push-box-2.h"
#include "BulletBody.h"
#include "Box.h"
#include "Robot.h"
#include "BulletPhysics.h"
#include "aux-rewards.h"


double static getDistanceBetweenPoints(double x1, double y1, double x2, double y2) {
	double distance = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
	return distance;
}


#define TargetX 12.0
#define TargetY -2.0

#define ground_x 0.0
#define ground_y -50.0
#define ground_z 0.0 

#define r1origin_x 0.0
#define r1origin_y 0.0

#define r2origin_x 3.0
#define r2origin_y 4.0

#define boxOrigin_x 3.0
#define boxOrigin_y 2.0

#define theta_o1 0.0
#define theta_o2 0.0

CPushBox2::CPushBox2(CConfigNode* pConfigNode)
{
	METADATA("World", "Push-Box-2");
	
	m_target_X = addStateVariable("target-x", "m", -20.0, 20.0);
	m_target_Y = addStateVariable("target-y", "m", -20.0, 20.0);
	m_rob1_X = addStateVariable("robot1-x", "m", -20.0, 20.0);
	m_rob1_Y = addStateVariable("robot1-y", "m", -20.0, 20.0);
	m_rob2_X = addStateVariable("robot2-x", "m", -20.0, 20.0);
	m_rob2_Y = addStateVariable("robot2-y", "m", -20.0, 20.0);

	m_box_X  = addStateVariable("box-x", "m", -20.0, 20.0);
	m_box_Y  = addStateVariable("box-y", "m", -20.0, 20.0);

	m_D_Br1X = addStateVariable("robot1-to-box-x", "m", -20.0, 20.0);
	m_D_Br1Y = addStateVariable("robot1-to-box-y", "m", -20.0, 20.0);
	m_D_Br2X = addStateVariable("robot2-to-box-x", "m", -20.0, 20.0);
	m_D_Br2Y = addStateVariable("robot2-to-box-y", "m", -20.0, 20.0);

	m_D_BtX = addStateVariable("box-to-target-x", "m", -20.0, 20.0);
	m_D_BtY = addStateVariable("box-to-target-y", "m", -20.0, 20.0);
	m_theta_r1 = addStateVariable("robot1-theta", "rad", -3.15, 3.15, true);
	m_theta_r2 = addStateVariable("robot2-theta", "rad", -3.15, 3.15, true);

	m_boxTheta = addStateVariable("box-theta", "rad", -3.15, 3.15, true);

	m_linear_vel_r1 = addActionVariable("robot1-v", "m/s", -2.0, 2.0);
	m_omega_r1 = addActionVariable("robot1-omega", "rad/s", -8.0, 8.0);
	m_linear_vel_r2 = addActionVariable("robot2-v", "m/s", -2.0, 2.0);
	m_omega_r2 = addActionVariable("robot2-omega", "rad/s", -8.0, 8.0);

	MASS_ROBOT = 1.1f;
	MASS_BOX = 6.9;
	MASS_GROUND = 0.f;
	MASS_TARGET = 0.1f;

	m_pBulletPhysics = new BulletPhysics();

	m_pBulletPhysics->initPhysics();

	///Creating static object, ground
	{
		m_pGround = new BulletBody(MASS_GROUND, btVector3(ground_x, ground_y, ground_z), new btBoxShape(btVector3(btScalar(50.), btScalar(50.), btScalar(50.))), false);
		m_pBulletPhysics->add(m_pGround);
	}

	/// Creating target point, static
	{
		m_pTarget = new BulletBody(MASS_TARGET, btVector3(TargetX, 0, TargetY), new btConeShape(btScalar(0.5), btScalar(0.001)), false);
		m_pTarget->getBody()->setCollisionFlags(m_pTarget->getBody()->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		m_pTarget->setAbsoluteStateVarIds(getStateDescriptor().getVarIndex("target-x")
			, getStateDescriptor().getVarIndex("target-y"), -1); 
		m_pBulletPhysics->add(m_pTarget);
	}

	///Creating dynamic box
	{
		m_pBox = new Box(MASS_BOX, btVector3(boxOrigin_x, 0, boxOrigin_y), new btBoxShape(btVector3(btScalar(0.6), btScalar(0.6), btScalar(0.6))), true);
		m_pBox->setAbsoluteStateVarIds(getStateDescriptor().getVarIndex("box-x")
			, getStateDescriptor().getVarIndex("box-y")
			, getStateDescriptor().getVarIndex("box-theta"));
		m_pBox->setRelativeStateVarIds(getStateDescriptor().getVarIndex("box-to-target-x")
			, getStateDescriptor().getVarIndex("box-to-target-y")
			, getStateDescriptor().getVarIndex("box-x")
			, getStateDescriptor().getVarIndex("box-y"));
		m_pBulletPhysics->add(m_pBox);
	}

	///creating  dynamic robot one
	{
		m_pRobot1 = new Robot(MASS_ROBOT, btVector3(r1origin_x, 0, r1origin_y), new btSphereShape(btScalar(0.5)));
		m_pRobot1->setAbsoluteStateVarIds(getStateDescriptor().getVarIndex("robot1-x")
			, getStateDescriptor().getVarIndex("robot1-y")
			, getStateDescriptor().getVarIndex("robot1-theta"));
		m_pRobot1->setActionIds(getActionDescriptor().getVarIndex("robot1-v")
			, getActionDescriptor().getVarIndex("robot1-omega"));
		m_pRobot1->setRelativeStateVarIds(getStateDescriptor().getVarIndex("robot1-to-box-x")
			, getStateDescriptor().getVarIndex("robot1-to-box-y")
			, getStateDescriptor().getVarIndex("box-x")
			, getStateDescriptor().getVarIndex("box-y"));
		m_pBulletPhysics->add(m_pRobot1);
	}

	///creating  dynamic robot two
	{
		m_pRobot2 = new Robot(MASS_ROBOT, btVector3(r2origin_x, 0, r2origin_y), new btSphereShape(btScalar(0.5)));
		m_pRobot2->setAbsoluteStateVarIds(getStateDescriptor().getVarIndex("robot2-x")
			, getStateDescriptor().getVarIndex("robot2-y")
			, getStateDescriptor().getVarIndex("robot2-theta"));
		m_pRobot2->setActionIds(getActionDescriptor().getVarIndex("robot2-v")
			, getActionDescriptor().getVarIndex("robot2-omega"));
		m_pRobot2->setRelativeStateVarIds(getStateDescriptor().getVarIndex("robot2-to-box-x")
			, getStateDescriptor().getVarIndex("robot2-to-box-y")
			, getStateDescriptor().getVarIndex("box-x")
			, getStateDescriptor().getVarIndex("box-y"));
		m_pBulletPhysics->add(m_pRobot2);
	}


	//the reward function
	m_pRewardFunction->addRewardComponent(new CDistanceReward2D(getStateDescriptor(),m_box_X, m_box_Y, m_target_X, m_target_Y));
	m_pRewardFunction->initialize();
}

void CPushBox2::reset(CState *s)
{
	m_pBulletPhysics->reset(s);
}

void CPushBox2::executeAction(CState *s, const CAction *a, double dt)
{
	m_pBulletPhysics->updateBulletState(s, a, dt);

	//Execute simulation
	m_pBulletPhysics->getDynamicsWorld()->stepSimulation(dt, 20);

	//Update
	m_pBulletPhysics->updateState(s);
}

CPushBox2::~CPushBox2()
{
	delete m_pBulletPhysics;
}