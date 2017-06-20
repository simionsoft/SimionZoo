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
	
	m_target_X = addStateVariable("targetX", "m", -20.0, 20.0);
	m_target_Y = addStateVariable("targetY", "m", -20.0, 20.0);
	m_rob1_X = addStateVariable("rx1", "m", -20.0, 20.0);
	m_rob1_Y = addStateVariable("ry1", "m", -20.0, 20.0);
	m_rob2_X = addStateVariable("rx2", "m", -20.0, 20.0);
	m_rob2_Y = addStateVariable("ry2", "m", -20.0, 20.0);

	m_box_X  = addStateVariable("bx", "m", -20.0, 20.0);
	m_box_Y  = addStateVariable("by", "m", -20.0, 20.0);

	m_D_Br1X = addStateVariable("dBr1X", "m", -20.0, 20.0);
	m_D_Br1Y = addStateVariable("dBr1Y", "m", -20.0, 20.0);
	m_D_Br2X = addStateVariable("dBr2X", "m", -20.0, 20.0);
	m_D_Br2Y = addStateVariable("dBr2Y", "m", -20.0, 20.0);

	m_D_BtX = addStateVariable("dBtX", "m", -20.0, 20.0);
	m_D_BtY = addStateVariable("dBtY", "m", -20.0, 20.0);
	m_theta_r1 = addStateVariable("theta1", "rad", -3.15, 3.15, true);
	m_theta_r2 = addStateVariable("theta2", "rad", -3.15, 3.15, true);

	m_linear_vel_r1 = addActionVariable("v1", "m/s", -2.0, 2.0);
	m_omega_r1 = addActionVariable("omega1", "rad/s", -8.0, 8.0);
	m_linear_vel_r2 = addActionVariable("v2", "m/s", -2.0, 2.0);
	m_omega_r2 = addActionVariable("omega2", "rad/s", -8.0, 8.0);

	MASS_ROBOT = 1.1f;
	MASS_BOX = 6.9;
	MASS_GROUND = 0.f;
	MASS_TARGET = 0.1f;

	m_pBulletPhysics = new BulletPhysics();

	m_pBulletPhysics->initPhysics();

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

	///creating  dynamic robot two
	{
		m_pRobot2 = new Robot(MASS_ROBOT, btVector3(r2origin_x, 0, r2origin_y), new btSphereShape(btScalar(0.5)), true);
		m_pBulletPhysics->getCollisionShape().push_back(m_pRobot2->getShape());
		m_pBulletPhysics->getDynamicsWorld()->addRigidBody(m_pRobot2->getBody());
	}


	//the reward function
	m_pRewardFunction->addRewardComponent(new CDistanceReward2D(getStateDescriptor(),m_box_X, m_box_Y, m_target_X, m_target_Y));
	m_pRewardFunction->initialize();
}

void CPushBox2::reset(CState *s)
{

	m_pRobot1->reset(s, r1origin_x, r1origin_y, m_rob1_X, m_rob1_Y);
	m_pRobot2->reset(s, r2origin_x, r2origin_y, m_rob2_X, m_rob2_Y);
	m_pBox->reset(s, boxOrigin_x, boxOrigin_y, m_box_X, m_box_Y);

	///set initial values to state variables
	s->set(m_theta_r1, theta_o1);
	s->set(m_theta_r2, theta_o2);

	///set relative coordinates
	s->set(m_D_Br1X, fabs(s->get(m_box_X) - s->get(m_rob1_X)));
	s->set(m_D_Br1Y, fabs(s->get(m_box_Y) - s->get(m_rob1_Y)));
	s->set(m_D_Br2X, fabs(s->get(m_box_X) - s->get(m_rob2_X)));
	s->set(m_D_Br2Y, fabs(s->get(m_box_Y) - s->get(m_rob2_Y)));
	s->set(m_D_BtX, fabs(s->get(m_box_X) - TargetX));
	s->set(m_D_BtY, fabs(s->get(m_box_Y) - TargetY));

	//target
	s->set(m_target_X, TargetX);
	s->set(m_target_Y, TargetY);

}

void CPushBox2::executeAction(CState *s, const CAction *a, double dt)
{

	double r1_theta;
	r1_theta = m_pRobot1->updateRobotMovement(a, s, "omega1", "v1", m_theta_r1, dt);
	double r2_theta;
	r2_theta = m_pRobot2->updateRobotMovement(a, s, "omega2", "v2", m_theta_r2, dt);

	//Execute simulation
	m_pBulletPhysics->getDynamicsWorld()->stepSimulation(dt, 20);

	//Update

	btTransform box_trans = m_pBox->setAbsoluteVariables(s, m_box_X, m_box_Y);
	m_pRobot1->setAbsoluteVariables(s, m_rob1_X, m_rob1_Y);
	m_pRobot2->setAbsoluteVariables(s, m_rob2_X, m_rob2_Y);
	
	m_pRobot1->setRelativeVariables(s, m_D_Br1X, m_D_Br1Y, box_trans.getOrigin().getX(), box_trans.getOrigin().getZ());
	m_pRobot2->setRelativeVariables(s, m_D_Br2X, m_D_Br2Y, box_trans.getOrigin().getX(), box_trans.getOrigin().getZ());
	m_pBox->setRelativeVariables(s, m_D_BtX, m_D_BtY, TargetX, TargetY);

		s->set(m_theta_r1, r1_theta);
		s->set(m_theta_r2, r2_theta);
}
