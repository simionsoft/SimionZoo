#include "stdafx.h"
#include "world-movebox2robots.h"
#include "app.h"
#include "noise.h"
#include  "GraphicSettings.h"
#include "BulletBody.h"
#pragma comment(lib,"opengl32.lib")

double static getDistanceBetweenPoints(double x1, double y1, double x2, double y2) {
	double distance = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
	return distance;
}

double static getDistanceOneDimension(double x1, double x2) {
	double dist = x2 - x1;
	if (dist < 0)
	{
		dist = dist * (-1);
	}
	return dist;
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

OpenGLGuiHelper *gui_helper;
CommonExampleOptions *opt_helper;

CMoveBox2Robots::CMoveBox2Robots(CConfigNode* pConfigNode)
{
	METADATA("World", "MoveBox-2Robots");
	

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
	m_theta_r1 = addStateVariable("theta1", "rad", -3.15, 3.15);
	m_theta_r2 = addStateVariable("theta2", "rad", -3.15, 3.15);

	m_linear_vel_r1 = addActionVariable("v1", "m/s", -2.0, 2.0);
	m_omega_r1 = addActionVariable("omega1", "rad/s", -8.0, 8.0);
	m_linear_vel_r2 = addActionVariable("v2", "m/s", -2.0, 2.0);
	m_omega_r2 = addActionVariable("omega2", "rad/s", -8.0, 8.0);

	MASS_ROBOT = 1.1f;
	MASS_BOX = 1.5f;
	MASS_GROUND = 0.f;
	MASS_TARGET = 0.1f;

	window_app = new SimpleOpenGL3App("Graphic Bullet Two Robots Interface", 1024, 768, true);

	///Graphic init
	gui_helper = new OpenGLGuiHelper(window_app, false);
	opt_helper = new CommonExampleOptions(gui_helper);

	rob2Builder = new BulletBuilder(opt_helper->m_guiHelper);
	rob2Builder->initializeBulletRequirements();
	rob2Builder->setOpenGLApp(window_app);

	///Creating static object, ground
	{
		m_Ground = new BulletBody(MASS_GROUND, ground_x, ground_y, ground_z, new btBoxShape(btVector3(btScalar(50.), btScalar(50.), btScalar(50.))), false);
		rob2Builder->getCollisionShape().push_back(m_Ground->getShape());
		rob2Builder->getDynamicsWorld()->addRigidBody(m_Ground->getBody());
	}

	/// Creating target point, static
	{
		m_Target = new BulletBody(MASS_TARGET, TargetX, 0, TargetY, new btConeShape(btScalar(0.5), btScalar(0.001)), false);
		m_Target->getBody()->setCollisionFlags(m_Target->getBody()->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		rob2Builder->getCollisionShape().push_back(m_Target->getShape());
		rob2Builder->getDynamicsWorld()->addRigidBody(m_Target->getBody());
	}

	///Creating dynamic box
	{
		m_Box = new BulletBody(MASS_BOX, boxOrigin_x, 0, boxOrigin_y, new btBoxShape(btVector3(btScalar(0.6), btScalar(0.6), btScalar(0.6))), true);
		rob2Builder->getCollisionShape().push_back(m_Box->getShape());
		rob2Builder->getDynamicsWorld()->addRigidBody(m_Box->getBody());
	}

	///creating  dynamic robot one
	{
		m_Robot1 = new BulletBody(MASS_ROBOT, r1origin_x, 0, r1origin_y, new btSphereShape(btScalar(0.5)), true);
		rob2Builder->getCollisionShape().push_back(m_Robot1->getShape());
		rob2Builder->getDynamicsWorld()->addRigidBody(m_Robot1->getBody());
	}

	///creating  dynamic robot two
	{
		m_Robot2 = new BulletBody(MASS_ROBOT, r2origin_x, 0, r2origin_y, new btSphereShape(btScalar(0.5)), true);
		rob2Builder->getCollisionShape().push_back(m_Robot2->getShape());
		rob2Builder->getDynamicsWorld()->addRigidBody(m_Robot2->getBody());
	}

	o_distBr1X = getDistanceOneDimension(r1origin_x, boxOrigin_x);
	o_distBr1Y = getDistanceOneDimension(r1origin_y, boxOrigin_y);

	o_distBr2X = getDistanceOneDimension(r2origin_x, boxOrigin_x);
	o_distBr2Y = getDistanceOneDimension(r2origin_y, boxOrigin_y);
	
	o_distBtX = getDistanceOneDimension(boxOrigin_x, TargetX);
	o_distBtY = getDistanceOneDimension(boxOrigin_y, TargetY);

	///Graphic init
	rob2Builder->generateGraphics(rob2Builder->getGuiHelper());

	//the reward function
	m_pRewardFunction->addRewardComponent(new CMoveBox2RobotsReward());
	m_pRewardFunction->initialize();
}

void CMoveBox2Robots::reset(CState *s)
{

	btTransform robot1Transform;
	btTransform robot2Transform;
	btTransform boxTransform;
	btTransform targetTransform;

	btQuaternion orientation = { 0.000000000, 0.000000000, 0.000000000, 1.00000000 };
	btVector3 zeroVector(0, 0, 0);

	m_Robot1->getBody()->clearForces();
	m_Robot1->getBody()->setLinearVelocity(zeroVector);
	m_Robot1->getBody()->setAngularVelocity(zeroVector);

	m_Robot2->getBody()->clearForces();
	m_Robot2->getBody()->setLinearVelocity(zeroVector);
	m_Robot2->getBody()->setAngularVelocity(zeroVector);

	m_Robot1->getBody()->getMotionState()->getWorldTransform(robot1Transform);
	m_Robot2->getBody()->getMotionState()->getWorldTransform(robot2Transform);
	m_Box->getBody()->getMotionState()->getWorldTransform(boxTransform);
	m_Target->getBody()->getMotionState()->getWorldTransform(targetTransform);

	/// reset robot1
	robot1Transform.setOrigin(btVector3(r1origin_x, 0.0, r1origin_y));
	m_Robot1->getBody()->setWorldTransform(robot1Transform);
	m_Robot1->getBody()->getMotionState()->setWorldTransform(robot1Transform);

	/// reset robot2
	robot2Transform.setOrigin(btVector3(r2origin_x, 0.0, r2origin_y));
	m_Robot2->getBody()->setWorldTransform(robot2Transform);
	m_Robot2->getBody()->getMotionState()->setWorldTransform(robot2Transform);

	///reset box
	boxTransform.setOrigin(btVector3(boxOrigin_x, 0.0, boxOrigin_y));
	boxTransform.setRotation(orientation);
	m_Box->getBody()->setWorldTransform(boxTransform);
	m_Box->getBody()->getMotionState()->setWorldTransform(boxTransform);

	/////reset target
	targetTransform.setOrigin(btVector3(TargetX, 0.0, TargetY));
	m_Target->getBody()->setWorldTransform(targetTransform);
	m_Target->getBody()->getMotionState()->setWorldTransform(targetTransform);

	///set initial values to state variables

	s->set(m_D_Br1X, o_distBr1X);
	s->set(m_D_Br1Y, o_distBr1Y);
	s->set(m_D_Br2X, o_distBr2X);
	s->set(m_D_Br2Y, o_distBr2Y);
	s->set(m_D_BtX, o_distBtX);
	s->set(m_D_BtY, o_distBtY);

	///set initial values to state variables
	s->set(m_rob1_X, r1origin_x);
	s->set(m_rob1_Y, r1origin_y);
	s->set(m_rob2_X, r2origin_x);
	s->set(m_rob2_Y, r2origin_y);
	s->set(m_box_X, boxOrigin_x);
	s->set(m_box_Y, boxOrigin_y);
	s->set(m_theta_r1, theta_o1);
	s->set(m_theta_r2, theta_o2);

}

void CMoveBox2Robots::executeAction(CState *s, const CAction *a, double dt)
{

	btTransform box_trans;
	btTransform rob1_trans;
	btTransform rob2_trans;
	double rob1_VelX, rob1_VelY;
	double rob2_VelX, rob2_VelY;

	double r1_omega = a->get("omega1");
	double r1_linear_vel = a->get("v1");
	double r2_omega = a->get("omega2");
	double r2_linear_vel = a->get("v2");

	double r1_theta = s->get(m_theta_r1);
	r1_theta += r1_omega*dt;

	double r2_theta = s->get(m_theta_r2);
	r2_theta += r2_omega*dt;

	if (r1_theta > SIMD_2_PI)
		r1_theta -= SIMD_2_PI;
	if (r1_theta< -SIMD_2_PI)
		r1_theta += SIMD_2_PI;

	if (r2_theta > SIMD_2_PI)
		r2_theta -= SIMD_2_PI;
	if (r2_theta < -SIMD_2_PI)
		r2_theta += SIMD_2_PI;


	rob1_VelX = cos(r1_theta)*r1_linear_vel;
	rob1_VelY = sin(r1_theta)*r1_linear_vel;
	m_Robot1->getBody()->setAngularVelocity(btVector3(0.0, r1_omega, 0.0));
	m_Robot1->getBody()->setLinearVelocity(btVector3(rob1_VelX, 0.0, rob1_VelY));

	rob2_VelX = cos(r2_theta)*r2_linear_vel;
	rob2_VelY = sin(r2_theta)*r2_linear_vel;
	m_Robot2->getBody()->setAngularVelocity(btVector3(0.0, r2_omega, 0.0));
	m_Robot2->getBody()->setLinearVelocity(btVector3(rob2_VelX, 0.0, rob2_VelY));

	//Execute simulation
	rob2Builder->getDynamicsWorld()->stepSimulation(dt, 20);

	//Update
	{
		m_Box->getBody()->getMotionState()->getWorldTransform(box_trans);
		m_Robot1->getBody()->getMotionState()->getWorldTransform(rob1_trans);
		m_Robot2->getBody()->getMotionState()->getWorldTransform(rob2_trans);

		s->set(m_box_X, float(box_trans.getOrigin().getX()));
		s->set(m_box_Y, float(box_trans.getOrigin().getZ()));

		s->set(m_rob1_X, double(rob1_trans.getOrigin().getX()));
		s->set(m_rob1_Y, double(rob1_trans.getOrigin().getZ()));
		s->set(m_rob2_X, double(rob2_trans.getOrigin().getX()));
		s->set(m_rob2_Y, double(rob2_trans.getOrigin().getZ()));

		s->set(m_D_Br1X, getDistanceOneDimension(rob1_trans.getOrigin().getX(), box_trans.getOrigin().getX()));
		s->set(m_D_Br1Y, getDistanceOneDimension(rob1_trans.getOrigin().getZ(), box_trans.getOrigin().getZ()));

		s->set(m_D_Br2X, getDistanceOneDimension(rob2_trans.getOrigin().getX(), box_trans.getOrigin().getX()));
		s->set(m_D_Br2Y, getDistanceOneDimension(rob2_trans.getOrigin().getZ(), box_trans.getOrigin().getZ()));

		s->set(m_D_BtX, getDistanceOneDimension(TargetX, box_trans.getOrigin().getX()));
		s->set(m_D_BtY, getDistanceOneDimension(TargetY, box_trans.getOrigin().getZ()));

		s->set(m_theta_r1, r1_theta);
		s->set(m_theta_r2, r2_theta);
	}

	//draw
	btVector3 printPosition = btVector3(box_trans.getOrigin().getX(), box_trans.getOrigin().getY() + 5, box_trans.getOrigin().getZ());
	if (CSimionApp::get()->pExperiment->isEvaluationEpisode())
	{
		rob2Builder->drawText3D("Evaluation episode", printPosition);
	}
	else
	{
		rob2Builder->drawText3D("Training episode", printPosition);
	}
	if (!CSimionApp::get()->isExecutedRemotely()) {
		rob2Builder->draw();
	}

}

#define BOX_ROBOT_REWARD_WEIGHT 3.0
#define BOX_TARGET_REWARD_WEIGHT 1.0
double CMoveBox2RobotsReward::getReward(const CState* s, const CAction* a, const CState* s_p)
{
	double boxAfterX = s_p->get("bx");
	double boxAfterY = s_p->get("by");

	double robot1X = s_p->get("rx1");
	double robot1Y = s_p->get("ry1");

	double robot2X = s_p->get("rx2");
	double robot2Y = s_p->get("ry2");

	double distanceBoxTarget = getDistanceBetweenPoints(TargetX, TargetY, boxAfterX, boxAfterY);
	double distanceBoxRobot1 = getDistanceBetweenPoints(robot1X, robot1Y, boxAfterX, boxAfterY);
	double distanceBoxRobot2 = getDistanceBetweenPoints(robot2X, robot2Y, boxAfterX, boxAfterY);

	if (robot1X >= 40.0 || robot1X <= -40.0 || robot1Y >= 40.0 || robot1Y <= -40.0 || robot2X >= 40.0 || robot2X <= -40.0 || robot2Y >= 40.0 || robot2Y <= -40.0)
	{
		CSimionApp::get()->pExperiment->setTerminalState();
		return -1;
	}
	distanceBoxRobot1 = std::max(distanceBoxRobot1, 0.0001);
	distanceBoxRobot2 = std::max(distanceBoxRobot2, 0.0001);
	distanceBoxTarget = std::max(distanceBoxTarget, 0.0001);

	double rewardBoxRobot1 = std::min(BOX_ROBOT_REWARD_WEIGHT*2.0, BOX_ROBOT_REWARD_WEIGHT / distanceBoxRobot1);
	double rewardBoxRobot2 = std::min(BOX_ROBOT_REWARD_WEIGHT*2.0, BOX_ROBOT_REWARD_WEIGHT / distanceBoxRobot2);
	double rewardBoxTarget = std::min(BOX_TARGET_REWARD_WEIGHT*2.0, BOX_TARGET_REWARD_WEIGHT / distanceBoxTarget);
	return rewardBoxRobot1 + rewardBoxRobot2 + rewardBoxTarget;
}

double CMoveBox2RobotsReward::getMin()
{
	return 0.0;
}

double CMoveBox2RobotsReward::getMax()
{
	return 10.0;
}