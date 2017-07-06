#include "../stdafx.h"
#include "../app.h"
#include "../noise.h"

#include "robot-control.h"
#include "Robot.h"
#include "BulletPhysics.h"
#include "BulletBody.h"
#include "aux-rewards.h"


#define TargetX 10.0
#define TargetY 3.0

#define robotOrigin_x 0.0
#define robotOrigin_y 0.0

#define ground_x 0.0
#define ground_y -50.0
#define ground_z 0.0 

#define theta_o 0.0

#define CNT_VEL 2.0

CRobotControl::CRobotControl(CConfigNode* pConfigNode)
{
	METADATA("World", "Robot-control");

	m_target_X = addStateVariable("target-x", "m", -20.0, 20.0);
	m_target_Y = addStateVariable("target-y", "m", -20.0, 20.0);

	m_rob1_X = addStateVariable("robot1-x", "m", -20.0, 20.0);
	m_rob1_Y = addStateVariable("robot1-y", "m", -20.0, 20.0);
	m_theta = addStateVariable("robot1-theta", "rad", -3.1415, 3.1415, true);

	m_linear_vel = addActionVariable("robot1-v", "m/s", -2.0, 2.0);
	m_omega = addActionVariable("robot1-omega", "rad", -8.0, 8.0);

	MASS_ROBOT = 0.5f;
	MASS_GROUND = 0.f;
	MASS_TARGET = 0.1f;

	m_pBulletPhysics = new BulletPhysics();
	m_pBulletPhysics->initPhysics();

	
	///Creating static object, ground
	{
		m_pGround = new BulletBody(MASS_GROUND, btVector3(ground_x, ground_y, ground_z)
			, new btBoxShape(btVector3(btScalar(50.), btScalar(50.), btScalar(50.))));
		m_pBulletPhysics->add(m_pGround);
	}

	/// Creating target point, static
	{
		m_pTarget = new BulletBody(MASS_TARGET, btVector3(TargetX, 0, TargetY), new btConeShape(btScalar(0.5)
			, btScalar(0.001)), btCollisionObject::CF_KINEMATIC_OBJECT);
		m_pTarget->setAbsoluteStateVarIds(getStateDescriptor().getVarIndex("target-x")
			, getStateDescriptor().getVarIndex("target-y"), -1);
		m_pBulletPhysics->add(m_pTarget);
	}

	///creating a dynamic robot  
	{
		m_pRobot1 = new Robot(MASS_ROBOT, btVector3(robotOrigin_x, 0, robotOrigin_y), new btSphereShape(0.5));
		m_pRobot1->setAbsoluteStateVarIds(getStateDescriptor().getVarIndex("robot1-x")
			, getStateDescriptor().getVarIndex("robot1-y")
			, getStateDescriptor().getVarIndex("robot1-theta"));
		m_pRobot1->setActionIds(getActionDescriptor().getVarIndex("robot1-v")
			, getActionDescriptor().getVarIndex("robot1-omega"));
		m_pBulletPhysics->add(m_pRobot1);
	}

	//the reward function
	m_pRewardFunction->addRewardComponent(new CDistanceReward2D(getStateDescriptor(),m_rob1_X,m_rob1_Y,m_target_X,m_target_Y));
	m_pRewardFunction->initialize();
}

void CRobotControl::reset(CState *s)
{
	m_pBulletPhysics->reset(s);
}

void CRobotControl::executeAction(CState *s, const CAction *a, double dt)
{
	btTransform trans;
	m_pBulletPhysics->updateBulletState(s, a, dt);

	//Update Robot1
	m_pBulletPhysics->stepSimulation((float)dt,20);

	//Update
	m_pBulletPhysics->updateState(s);
}

CRobotControl::~CRobotControl()
{
	delete m_pBulletPhysics;
}

