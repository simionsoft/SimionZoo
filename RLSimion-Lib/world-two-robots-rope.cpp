#include "stdafx.h"
#include "world-two-robots-rope.h"
#include "app.h"
#include "noise.h"
#include "GraphicSettings.h"
#include "BulletBody.h"
#pragma comment(lib,"opengl32.lib")

double static getDistanceBetweenPoints(double x1, double y1, double x2, double y2) {
	double distance = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
	return distance;
}

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

#define r2origin_x 3.0
#define r2origin_y 4.0

#define boxOrigin_x 3.0
#define boxOrigin_y 2.0

#define theta_o1 0.0
#define theta_o2 0.0

OpenGLGuiHelper *helper_gui;
CommonExampleOptions *help_opt;
btSoftBodyWorldInfo m_sBodyWorldInfo;
btSoftBodyWorldInfo* s_pBodyInfo = &m_sBodyWorldInfo;

CRope2Robots::CRope2Robots(CConfigNode* pConfigNode)
{
	METADATA("World", "MoveRopeRobots");

	m_rob1_X = addStateVariable("rx1", "m", -20.0, 20.0);
	m_rob1_Y = addStateVariable("ry1", "m", -20.0, 20.0);
	m_rob2_X = addStateVariable("rx2", "m", -20.0, 20.0);
	m_rob2_Y = addStateVariable("ry2", "m", -20.0, 20.0);

	m_box_X = addStateVariable("bx", "m", -20.0, 20.0);
	m_box_Y = addStateVariable("by", "m", -20.0, 20.0);

	m_theta_r1 = addStateVariable("theta1", "rad", -3.15, 3.15);
	m_theta_r2 = addStateVariable("theta2", "rad", -3.15, 3.15);
	m_boxTheta = addStateVariable("boxTheta", "rad", -3.15, 3.15);
	m_D_BtX = addStateVariable("dBtX", "m", -20.0, 20.0);
	m_D_BtY = addStateVariable("dBtY", "m", -20.0, 20.0);

	m_D_Br1X = addStateVariable("dBr1X", "m", -20.0, 20.0);
	m_D_Br1Y = addStateVariable("dBr1Y", "m", -20.0, 20.0);
	m_D_Br2X = addStateVariable("dBr2X", "m", -20.0, 20.0);
	m_D_Br2Y = addStateVariable("dBr2Y", "m", -20.0, 20.0);

	m_linear_vel_r1 = addActionVariable("v1", "m/s", -2.0, 2.0);
	m_omega_r1 = addActionVariable("omega1", "rad/s", -8.0, 8.0);
	m_linear_vel_r2 = addActionVariable("v2", "m/s", -2.0, 2.0);
	m_omega_r2 = addActionVariable("omega2", "rad/s", -8.0, 8.0);

	MASS_ROBOT = 1.1f;
	MASS_BOX = 6.9;
	MASS_GROUND = 0.f;
	MASS_TARGET = 0.1f;

	app_window = new SimpleOpenGL3App("Graphic Bullet Two Robots Rope Interface", 1024, 768, true);

	///Graphic init
	helper_gui = new OpenGLGuiHelper(app_window, false);
	help_opt = new CommonExampleOptions(helper_gui);

	robRopeBuilder = new BulletBuilder(help_opt->m_guiHelper);
	robRopeBuilder->initSoftBullet(s_pBodyInfo);
	robRopeBuilder->setOpenGLApp(app_window);

	///Creating static object, ground
	{
		m_Ground = new BulletBody(MASS_GROUND, btVector3(ground_x, ground_y, ground_z), new btBoxShape(btVector3(btScalar(50.), btScalar(50.), btScalar(50.))), false);
		robRopeBuilder->getCollisionShape().push_back(m_Ground->getShape());
		robRopeBuilder->getDynamicsWorld()->addRigidBody(m_Ground->getBody());
	}

	/// Creating target point, static
	{
		m_Target = new BulletBody(MASS_TARGET, btVector3(TargetX, 0, TargetY), new btConeShape(btScalar(0.5), btScalar(0.001)), false);
		m_Target->getBody()->setCollisionFlags(m_Target->getBody()->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		robRopeBuilder->getCollisionShape().push_back(m_Target->getShape());
		robRopeBuilder->getDynamicsWorld()->addRigidBody(m_Target->getBody());
	}

	///Creating dynamic box
	{
		m_Box = new BulletBody(MASS_BOX, btVector3(boxOrigin_x, 0, boxOrigin_y), new btBoxShape(btVector3(btScalar(0.6), btScalar(0.6), btScalar(0.6))), true);
		robRopeBuilder->getCollisionShape().push_back(m_Box->getShape());
		robRopeBuilder->getDynamicsWorld()->addRigidBody(m_Box->getBody());
	}

	///creating  dynamic robot one
	{
		m_Robot1 = new BulletBody(MASS_ROBOT, btVector3(r1origin_x, 0, r1origin_y), new btSphereShape(btScalar(0.5)), true);
		robRopeBuilder->getCollisionShape().push_back(m_Robot1->getShape());
		robRopeBuilder->getDynamicsWorld()->addRigidBody(m_Robot1->getBody());
	}

	///creating  dynamic robot two
	{
		m_Robot2 = new BulletBody(MASS_ROBOT, btVector3(r2origin_x, 0, r2origin_y), new btSphereShape(btScalar(0.5)), true);
		robRopeBuilder->getCollisionShape().push_back(m_Robot2->getShape());
		robRopeBuilder->getDynamicsWorld()->addRigidBody(m_Robot2->getBody());
	}

	/// creating an union with rope between robot and box
	{
		robRopeBuilder->connectWithRope(m_Robot1->getBody(), m_Box->getBody(), s_pBodyInfo);
		robRopeBuilder->connectWithRope(m_Robot2->getBody(), m_Box->getBody(), s_pBodyInfo);
	}

	///Graphic init
	robRopeBuilder->generateGraphics(robRopeBuilder->getGuiHelper());

	//the reward function
	m_pRewardFunction->addRewardComponent(new CRope2RobotsReward());
	m_pRewardFunction->initialize();
}

void CRope2Robots::reset(CState *s)
{
	if (CSimionApp::get()->pExperiment->isEvaluationEpisode())
	{
		m_Robot1->reset(s, r1origin_x, r1origin_y, m_rob1_X, m_rob1_Y);
		m_Robot2->reset(s, r2origin_x, r2origin_y, m_rob2_X, m_rob2_Y);
		m_Box->reset(s, boxOrigin_x, boxOrigin_y, m_box_X, m_box_Y);

		s->set(m_theta_r1, theta_o1);
		s->set(m_theta_r2, theta_o2);
		s->set(m_boxTheta, 0.0);
	}
	else
	{
		double boxOrX = boxOrigin_x + getRand(2.0);
		double boxOrY = boxOrigin_y + getRand(2.0);
		double rob1OrX = r1origin_x + getRand(2.0);
		double rob1OrY = r1origin_y + getRand(2.0);
		double rob2OrX = r2origin_x + getRand(2.0);
		double rob2OrY = r2origin_y + getRand(2.0);

		m_Box->reset(s, boxOrX, boxOrY, m_box_X, m_box_Y);
		m_Robot1->reset(s, rob1OrX, rob1OrY, m_rob1_X, m_rob1_Y);
		m_Robot2->reset(s, rob2OrX, rob2OrY, m_rob2_X, m_rob2_Y);

		s->set(m_theta_r1, theta_o1 + getRand(1.0));
		s->set(m_theta_r2, theta_o2 + getRand(1.0));
		s->set(m_boxTheta, 0.0);
	}

	//set relative coordinates
	s->set(m_D_Br1X, fabs(s->get(m_box_X) - s->get(m_rob1_X)));
	s->set(m_D_Br1Y, fabs(s->get(m_box_Y) - s->get(m_rob1_Y)));
	s->set(m_D_Br2X, fabs(s->get(m_box_X) - s->get(m_rob2_X)));
	s->set(m_D_Br2Y, fabs(s->get(m_box_Y) - s->get(m_rob2_Y)));
	s->set(m_D_BtX, fabs(s->get(m_box_X) - TargetX));
	s->set(m_D_BtY, fabs(s->get(m_box_Y) - TargetY));

}

void CRope2Robots::executeAction(CState *s, const CAction *a, double dt)
{

	double r1_theta;
	r1_theta = m_Robot1->updateRobotMovement(a, s, "omega1", "v1", m_theta_r1, dt);
	double r2_theta;
	r2_theta = m_Robot2->updateRobotMovement(a, s, "omega2", "v2", m_theta_r2, dt);

	//Execute simulation
	robRopeBuilder->getDynamicsWorld()->stepSimulation(dt, 20);

	//Update

	btTransform box_trans = m_Box->setAbsoluteVariables(s, m_box_X, m_box_Y);
	m_Robot1->setAbsoluteVariables(s, m_rob1_X, m_rob1_Y);
	m_Robot2->setAbsoluteVariables(s, m_rob2_X, m_rob2_Y);

	m_Robot1->setRelativeVariables(s, m_D_Br1X, m_D_Br1X, false, NULL, NULL, box_trans.getOrigin().getX(), box_trans.getOrigin().getZ());
	m_Robot2->setRelativeVariables(s, m_D_Br2X, m_D_Br2Y, false, NULL, NULL, box_trans.getOrigin().getX(), box_trans.getOrigin().getZ());
	m_Box->setRelativeVariables(s, m_D_BtX, m_D_BtY, true, TargetX, TargetY);

	s->set(m_theta_r1, r1_theta);
	s->set(m_theta_r2, r2_theta);
	btScalar yaw, pitch, roll;
	box_trans.getBasis().getEulerYPR(yaw, pitch, roll);
	if (pitch < SIMD_2_PI) pitch += SIMD_2_PI;
	else if (pitch > SIMD_2_PI) pitch -= SIMD_2_PI;
	s->set(m_boxTheta, (double)yaw);

	//draw
	btVector3 printPosition = btVector3(box_trans.getOrigin().getX(), box_trans.getOrigin().getY() + 5, box_trans.getOrigin().getZ());
	if (CSimionApp::get()->pExperiment->isEvaluationEpisode())
	{
		robRopeBuilder->drawText3D("Evaluation episode", printPosition);
		robRopeBuilder->drawSoftWorld();
	}
	else
	{
		robRopeBuilder->drawText3D("Training episode", printPosition);
	}
	if (!CSimionApp::get()->isExecutedRemotely()) {
		//robRopeBuilder->drawSoftWorld();
	}

}

double CRope2RobotsReward::getReward(const CState* s, const CAction* a, const CState* s_p)
{
	double boxAfterX = s_p->get("bx");
	double boxAfterY = s_p->get("by");

	double robot1X = s_p->get("rx1");
	double robot1Y = s_p->get("ry1");

	double robot2X = s_p->get("rx2");
	double robot2Y = s_p->get("ry2");


	if (robot1X >= 20.0 || robot1X <= -20.0 || robot1Y >= 20.0 || robot1Y <= -20.0 || robot2X >= 20.0 || robot2X <= -20.0 || robot2Y >= 20.0 || robot2Y <= -20.0)
	{
		CSimionApp::get()->pExperiment->setTerminalState();
		return -1;
	}
	double distanceBoxTarget = getDistanceBetweenPoints(TargetX, TargetY, boxAfterX, boxAfterY);
	if (distanceBoxTarget < 1)
		return 1 + (1 - distanceBoxTarget);
	else
		return 1 / distanceBoxTarget;
}

double CRope2RobotsReward::getMin()
{
	return 0.0;
}

double CRope2RobotsReward::getMax()
{
	return 2.0;
}

CRope2Robots::~CRope2Robots()
{
	delete helper_gui;
	delete help_opt;
	delete s_pBodyInfo;
	delete m_Ground;
	delete m_Robot1;
	delete m_Robot2;
	delete m_Box;
	delete m_Target;
}