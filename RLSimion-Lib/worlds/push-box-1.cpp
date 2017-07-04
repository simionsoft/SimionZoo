#include "../stdafx.h"
#include "../app.h"
#include "../noise.h"

#include "push-box-1.h"
#include "BulletPhysics.h"
#include "BulletBody.h"
#include "Box.h"
#include "Robot.h"
#include "aux-rewards.h"

double static getRand(double range)
{
	return (-range*0.5) + (range)*getRandomValue();
}

#define TargetX 10.0
#define TargetY 3.0

#define ground_x 0.0
#define ground_y -1.0
#define ground_z 0.0 

#define robotOrigin_x 0.0
#define robotOrigin_y 0.0

#define boxOrigin_x 3.0
#define boxOrigin_y 0.0

#define theta_o 0.0

CPushBox1::CPushBox1(CConfigNode* pConfigNode)
{
	METADATA("World", "Push-Box-1");

	m_target_X = addStateVariable("target-x", "m", -20.0, 20.0);
	m_target_Y = addStateVariable("target-y", "m", -20.0, 20.0);
	m_rob1_X = addStateVariable("robot1-x", "m", -10.0, 10.0);
	m_rob1_Y = addStateVariable("robot1-y", "m", -10.0, 10.0);
	m_box_X = addStateVariable("box-x", "m", -10.0, 10.0);
	m_box_Y = addStateVariable("box-y", "m", -10.0, 10.0);

	m_D_BrX = addStateVariable("robot1-to-box-x", "m", -10.0, 10.0);
	m_D_BrY = addStateVariable("robot1-to-box-y", "m", -10.0, 10.0);
	m_D_BtX = addStateVariable("box-to-target-x", "m", -10.0, 10.0);
	m_D_BtY = addStateVariable("box-to-target-y", "m", -10.0, 10.0);
	m_theta = addStateVariable("robot1-theta", "rad", -3.15, 3.15, true);
	m_boxTheta = addStateVariable("box-theta", "rad", -3.15, 3.15, true);

	m_linear_vel = addActionVariable("robot1-v", "m/s", -2.0, 2.0);
	m_omega = addActionVariable("robot1-omega", "rad/s", -8.0, 8.0);

	MASS_ROBOT = 1.5f;
	MASS_BOX = 1.f;
	MASS_GROUND = 0.f;
	MASS_TARGET = 0.1f;

	//Init Bullet
	m_pBulletPhysics = new BulletPhysics();

	m_pBulletPhysics->initPhysics();

	///Creating static object, ground
	{
		m_pGround = new BulletBody(MASS_GROUND, btVector3(ground_x, ground_y, ground_z), new btBoxShape(btVector3(btScalar(20.), btScalar(1.), btScalar(20.))), false);
		m_pBulletPhysics->add(m_pGround);
	}
	///Creating static object, walls
	{
		m_pWall1 = new BulletBody(MASS_GROUND, btVector3(12.0, 1.0, 0.0), new btBoxShape(btVector3(1.0, 2.0, 13.)), false);
		m_pBulletPhysics->add(m_pWall1);
		m_pWall2 = new BulletBody(MASS_GROUND, btVector3(-12.0, 1.0, 0.0), new btBoxShape(btVector3(1.0, 2.0, 13.)), false);
		m_pBulletPhysics->add(m_pWall2);
		m_pWall3 = new BulletBody(MASS_GROUND, btVector3(1.0, 1.0, 12.0), new btBoxShape(btVector3(12.0, 2.0, 1.0)), false);
		m_pBulletPhysics->add(m_pWall3);
		m_pWall4 = new BulletBody(MASS_GROUND, btVector3(1.0, 1.0, -12.0), new btBoxShape(btVector3(12.0, 2.0, 1.0)), false);
		m_pBulletPhysics->add(m_pWall4);
	}

	/// Creating target point, static
	{
		m_pTarget = new BulletBody(MASS_TARGET, btVector3(TargetX, 0.001, TargetY), new btConeShape(0.5, 0.001), false);
		m_pTarget->getBody()->setCollisionFlags(m_pTarget->getBody()->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		m_pTarget->setAbsoluteStateVarIds(getStateDescriptor().getVarIndex("target-x")
			, getStateDescriptor().getVarIndex("target-y"), -1);
		m_pBulletPhysics->add(m_pTarget);
	}

	///Creating dynamic box
	{
		m_pBox = new Box(MASS_BOX, btVector3(boxOrigin_x, 0, boxOrigin_y), new btBoxShape(btVector3(0.6, 0.6, 0.6)), true);
		m_pBox->setAbsoluteStateVarIds(getStateDescriptor().getVarIndex("box-x")
			, getStateDescriptor().getVarIndex("box-y")
			, getStateDescriptor().getVarIndex("box-theta"));
		m_pBox->setRelativeStateVarIds(getStateDescriptor().getVarIndex("box-to-target-x")
			, getStateDescriptor().getVarIndex("box-to-target-y")
			, getStateDescriptor().getVarIndex("box-x")
			, getStateDescriptor().getVarIndex("box-y"));
		m_pBulletPhysics->add(m_pBox);
	}

	///creating a dynamic robot  
	{
		m_pRobot1 = new Robot(MASS_ROBOT, btVector3(robotOrigin_x, 0, robotOrigin_y), new btSphereShape(0.5));
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

	//the reward function
	m_pRewardFunction->addRewardComponent(new CDistanceReward2D(getStateDescriptor(), m_box_X,m_box_Y,m_target_X,m_target_Y));
	m_pRewardFunction->initialize();
}


void CPushBox1::reset(CState *s)
{
	if (1)//CSimionApp::get()->pExperiment->isEvaluationEpisode())
	{
	}
	else
	{
		m_pBox->setOrigin(boxOrigin_x + getRand(2.0), boxOrigin_y + getRand(2.0), theta_o + getRand(1.0));
		m_pRobot1->setOrigin(robotOrigin_x + getRand(2.0), robotOrigin_y + getRand(2.0), theta_o + getRand(1.0));
	}
	m_pBulletPhysics->reset(s);
}

void CPushBox1::executeAction(CState *s, const CAction *a, double dt)
{
	m_pBulletPhysics->updateBulletState(s, a, dt);
	
	//Execute simulation
	m_pBulletPhysics->stepSimulation((float)dt, 20);

	//Update
	m_pBulletPhysics->updateState(s);
}

CPushBox1::~CPushBox1()
{
	delete m_pBulletPhysics;
}