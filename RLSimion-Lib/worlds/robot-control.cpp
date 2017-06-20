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

	m_target_X = addStateVariable("targetX", "m", -20.0, 20.0);
	m_target_Y = addStateVariable("targetY", "m", -20.0, 20.0);

	m_rob1_X = addStateVariable("rx1", "m", -20.0, 20.0);
	m_rob1_Y = addStateVariable("ry1", "m", -20.0, 20.0);
	m_theta = addStateVariable("theta", "rad", -3.1415, 3.1415, true);

	m_linear_vel = addActionVariable("v", "m/s", -2.0, 2.0);
	m_omega = addActionVariable("omega", "rad", -8.0, 8.0);

	MASS_ROBOT = 0.5f;
	MASS_GROUND = 0.f;
	MASS_TARGET = 0.1f;

	m_pBulletPhysics = new BulletPhysics();

	m_pBulletPhysics->initPhysics();

	
	///Creating static object, ground
	{
		btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(50.), btScalar(50.), btScalar(50.)));
		// "false" parameter will notify the constructor not to disable activation state
		ground_bb = new BulletBody(MASS_GROUND, btVector3(ground_x, ground_y, ground_z), groundShape, false);
		m_pBulletPhysics->getCollisionShape().push_back(ground_bb->getShape());
		m_pBulletPhysics->getDynamicsWorld()->addRigidBody(ground_bb->getBody());
	}

	/// Creating target point, static
	{
		btCollisionShape* targetShape = new btConeShape(btScalar(0.5), btScalar(5.5));
		target_bb = new BulletBody(MASS_TARGET, btVector3(TargetX, 0, TargetY+1), targetShape, false);
		target_bb->getBody()->setCollisionFlags(2);
		m_pBulletPhysics->getCollisionShape().push_back(target_bb->getShape());
		m_pBulletPhysics->getDynamicsWorld()->addRigidBody(target_bb->getBody());
	}

	///creating a dynamic robot  
	{
		m_Robot = new Robot(MASS_ROBOT, btVector3(robotOrigin_x, 0, robotOrigin_y), new btSphereShape(0.5), true);
		m_pBulletPhysics->getCollisionShape().push_back(m_Robot->getShape());
		m_pBulletPhysics->getDynamicsWorld()->addRigidBody(m_Robot->getBody());
	}

	//the reward function
	m_pRewardFunction->addRewardComponent(new CDistanceReward2D(getStateDescriptor(),m_rob1_X,m_rob1_Y,m_target_X,m_target_Y));
	m_pRewardFunction->initialize();
}

void CRobotControl::reset(CState *s)
{
	btTransform robotTransform;

	{
		m_Robot->reset(s, robotOrigin_x, robotOrigin_y, m_rob1_X, m_rob1_Y);

		///set initial values to state variables
		s->set(m_theta, theta_o);
		s->set(m_target_X, TargetX);
		s->set(m_target_Y, TargetY);
	}
}

void CRobotControl::executeAction(CState *s, const CAction *a, double dt)
{
	double theta= m_Robot->updateRobotMovement(a, s, "omega", "v", m_theta, dt);

	//Update Robot1
	m_pBulletPhysics->getDynamicsWorld()->stepSimulation(dt,20);

	//Update
	m_Robot->setAbsoluteVariables(s, m_rob1_X, m_rob1_Y);

	s->set(m_theta, theta);
}

CRobotControl::~CRobotControl()
{
	delete m_pBulletPhysics;
	delete ground_bb;
	delete target_bb;
}

