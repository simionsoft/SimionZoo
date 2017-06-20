#include "../stdafx.h"
#include "../app.h"
#include "../noise.h"

#include "pull-box-1.h"
#include "BulletBody.h"
#include "Robot.h"
#include "Box.h"
#include "BulletPhysics.h"
#include "Rope.h"
#include "aux-rewards.h"

double static getRand(double range)
{
	return (-range*0.5) + (range)*getRandomValue();
}

#define TargetX 12.0
#define TargetY -2.0

#define ground_x 0.0
#define ground_y -50.0
#define ground_z 0.0 

#define r1origin_x 0.0
#define r1origin_y 0.0

#define boxOrigin_x 3.0
#define boxOrigin_y 2.0

#define theta_o1 0.0

CPullBox1::CPullBox1(CConfigNode* pConfigNode)
{
	METADATA("World", "Pull-Box-1");
	m_target_X = addStateVariable("targetX", "m", -20.0, 20.0);
	m_target_Y = addStateVariable("targetY", "m", -20.0, 20.0);

	m_rob1_X = addStateVariable("rx1", "m", -20.0, 20.0);
	m_rob1_Y = addStateVariable("ry1", "m", -20.0, 20.0);

	m_box_X = addStateVariable("bx", "m", -20.0, 20.0);
	m_box_Y = addStateVariable("by", "m", -20.0, 20.0);

	m_theta_r1 = addStateVariable("theta", "rad", -3.15, 3.15, true);
	m_boxTheta = addStateVariable("boxTheta", "rad", -3.15, 3.15, true);
	m_D_BtX = addStateVariable("dBtX", "m", -20.0, 20.0);
	m_D_BtY = addStateVariable("dBtY", "m", -20.0, 20.0);

	m_D_Br1X = addStateVariable("dBr1X", "m", -6.0, 6.0);
	m_D_Br1Y = addStateVariable("dBr1Y", "m", -6.0, 6.0);

	m_linear_vel_r1 = addActionVariable("v", "m/s", -2.0, 2.0);
	m_omega_r1 = addActionVariable("omega", "rad/s", -8.0, 8.0);

	MASS_ROBOT = 1.1f;
	MASS_BOX = 6.9;
	MASS_GROUND = 0.f;
	MASS_TARGET = 0.1f;

	///Init Bullet
	m_pBulletPhysics = new BulletPhysics();

	m_pBulletPhysics->initSoftPhysics();

	///Creating static object, ground
	{
		m_pGround = new BulletBody(MASS_GROUND, btVector3(ground_x, ground_y, ground_z), new btBoxShape(btVector3(btScalar(50.), btScalar(50.), btScalar(50.))), false);
		m_pBulletPhysics->getCollisionShape().push_back(m_pGround->getShape());
		m_pBulletPhysics->getDynamicsWorld()->addRigidBody(m_pGround->getBody());
	}

	/// Creating target point, static
	{
		m_pTarget = new BulletBody(MASS_TARGET, btVector3(TargetX, 0, TargetY), new btConeShape(btScalar(0.5), btScalar(0.001)), false);
		m_pTarget->getBody()->setCollisionFlags(m_pTarget->getBody()->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		m_pBulletPhysics->getCollisionShape().push_back(m_pTarget->getShape());
		m_pBulletPhysics->getDynamicsWorld()->addRigidBody(m_pTarget->getBody());
	}

	///Creating dynamic box
	{
		m_pBox = new Box(MASS_BOX, btVector3(boxOrigin_x, 0, boxOrigin_y), new btBoxShape(btVector3(btScalar(0.6), btScalar(0.6), btScalar(0.6))), true);
		m_pBulletPhysics->getCollisionShape().push_back(m_pBox->getShape());
		m_pBulletPhysics->getDynamicsWorld()->addRigidBody(m_pBox->getBody());
	}

	///creating  dynamic robot one
	{
		m_pRobot1 = new Robot(MASS_ROBOT, btVector3(r1origin_x, 0, r1origin_y), new btSphereShape(btScalar(0.5)), true);
		m_pBulletPhysics->getCollisionShape().push_back(m_pRobot1->getShape());
		m_pBulletPhysics->getDynamicsWorld()->addRigidBody(m_pRobot1->getBody());
	}

	/// creating an union with rope between robot and box
	{
		m_pBulletPhysics->connectWithRope(m_pRobot1->getBody(), m_pBox->getBody());
		m_Rope = new Rope(this, m_pBulletPhysics->getSoftBodiesArray());
	}

	//the reward function
	m_pRewardFunction->addRewardComponent(new CDistanceReward2D(getStateDescriptor(),m_box_X, m_box_Y, m_target_X, m_target_Y));
	m_pRewardFunction->initialize();
}

void CPullBox1::reset(CState *s)
{
	if (CSimionApp::get()->pExperiment->isEvaluationEpisode())
	{
		m_pRobot1->reset(s, r1origin_x, r1origin_y, m_rob1_X, m_rob1_Y);
		m_pBox->reset(s, boxOrigin_x, boxOrigin_y, m_box_X, m_box_Y);

		s->set(m_theta_r1, theta_o1);
		s->set(m_boxTheta, 0.0);
	}
	else
	{
		double boxOrX = boxOrigin_x + getRand(2.0);
		double boxOrY = boxOrigin_y + getRand(2.0);
		double rob1OrX = r1origin_x + getRand(2.0);
		double rob1OrY = r1origin_y + getRand(2.0);

		m_pBox->reset(s, boxOrX, boxOrY, m_box_X, m_box_Y);
		m_pRobot1->reset(s, rob1OrX, rob1OrY, m_rob1_X, m_rob1_Y);

		s->set(m_theta_r1, theta_o1 + getRand(1.0));
		s->set(m_boxTheta, 0.0);
	}

	//set relative coordinates
	s->set(m_D_Br1X, fabs(s->get(m_box_X) - s->get(m_rob1_X)));
	s->set(m_D_Br1Y, fabs(s->get(m_box_Y) - s->get(m_rob1_Y)));
	s->set(m_D_BtX, fabs(s->get(m_box_X) - TargetX));
	s->set(m_D_BtY, fabs(s->get(m_box_Y) - TargetY));

	//target
	s->set(m_target_X, TargetX);
	s->set(m_target_Y, TargetY);

	//rope
	m_Rope->updateRopePoints(s, m_pBulletPhysics->getSoftBodiesArray());
}

void CPullBox1::executeAction(CState *s, const CAction *a, double dt)
{

	double r1_theta;
	r1_theta = m_pRobot1->updateRobotMovement(a, s, "omega", "v", m_theta_r1, dt);

	//Execute simulation
	m_pBulletPhysics->simulate(dt, 20);

	//Update

	btTransform box_trans = m_pBox->setAbsoluteVariables(s, m_box_X, m_box_Y);
	m_pRobot1->setAbsoluteVariables(s, m_rob1_X, m_rob1_Y);

	m_pRobot1->setRelativeVariables(s, m_D_Br1X, m_D_Br1Y, box_trans.getOrigin().getX(), box_trans.getOrigin().getZ());
	m_pBox->setRelativeVariables(s, m_D_BtX, m_D_BtY, TargetX, TargetY);

	m_Rope->updateRopePoints(s, m_pBulletPhysics->getSoftBodiesArray());

	s->set(m_theta_r1, r1_theta);
	btScalar yaw, pitch, roll;
	box_trans.getBasis().getEulerYPR(yaw, pitch, roll);
	if (pitch < SIMD_2_PI) pitch += SIMD_2_PI;
	else if (pitch > SIMD_2_PI) pitch -= SIMD_2_PI;
	s->set(m_boxTheta, (double)yaw);
}

CPullBox1::~CPullBox1()
{
	delete m_pGround;
	delete m_pBox;
	delete m_pRobot1;
	delete m_pTarget;
}