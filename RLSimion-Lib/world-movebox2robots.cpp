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
	MASS_BOX = 6.9;
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

	m_Robot1->updateResetVariables(s, r1origin_x, r1origin_y, m_rob1_X, m_rob1_Y);
	m_Robot2->updateResetVariables(s, r2origin_x, r2origin_y, m_rob2_X, m_rob2_Y);
	m_Box->updateResetVariables(s, boxOrigin_x, boxOrigin_y, m_box_X, m_box_Y);

	///set initial values to distance variables

	s->set(m_D_Br1X, o_distBr1X);
	s->set(m_D_Br1Y, o_distBr1Y);
	s->set(m_D_Br2X, o_distBr2X);
	s->set(m_D_Br2Y, o_distBr2Y);
	s->set(m_D_BtX, o_distBtX);
	s->set(m_D_BtY, o_distBtY);

	///set initial values to state variables
	s->set(m_theta_r1, theta_o1);
	s->set(m_theta_r2, theta_o2);

}

void CMoveBox2Robots::executeAction(CState *s, const CAction *a, double dt)
{

	double r1_theta;
	r1_theta = m_Robot1->updateRobotMovement(a, s, "omega1", "v1", m_theta_r1, dt);
	double r2_theta;
	r2_theta = m_Robot2->updateRobotMovement(a, s, "omega2", "v2", m_theta_r2, dt);

	//Execute simulation
	rob2Builder->getDynamicsWorld()->stepSimulation(dt, 20);

	//Update

	btTransform box_trans = m_Box->setAbsoluteActionVariables(s, m_box_X, m_box_Y);
	m_Robot1->setAbsoluteActionVariables(s, m_rob1_X, m_rob1_Y);
	m_Robot2->setAbsoluteActionVariables(s, m_rob2_X, m_rob2_Y);
	
	m_Robot1->setRelativeActionVariables(s, m_D_Br1X, m_D_Br1X, false, NULL, NULL, box_trans.getOrigin().getX(), box_trans.getOrigin().getZ());
	m_Robot2->setRelativeActionVariables(s, m_D_Br2X, m_D_Br2Y, false, NULL, NULL, box_trans.getOrigin().getX(), box_trans.getOrigin().getZ());
	m_Box->setRelativeActionVariables(s, m_D_BtX, m_D_BtY, true, TargetX, TargetY);

		s->set(m_theta_r1, r1_theta);
		s->set(m_theta_r2, r2_theta);

	//draw
	btVector3 printPosition = btVector3(box_trans.getOrigin().getX(), box_trans.getOrigin().getY() + 5, box_trans.getOrigin().getZ());
	if (CSimionApp::get()->pExperiment->isEvaluationEpisode())
	{
		rob2Builder->drawText3D("Evaluation episode", printPosition);
		rob2Builder->draw();
	}
	else
	{
		rob2Builder->drawText3D("Training episode", printPosition);
	}
	if (!CSimionApp::get()->isExecutedRemotely()) {
		//rob2Builder->draw();
	}

}

#define BOX_ROBOT_REWARD_WEIGHT 1.0
#define BOX_TARGET_REWARD_WEIGHT 3.0
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

	/*if (!CSimionApp::get()->pExperiment->isEvaluationEpisode())
	{
		if (distanceBoxTarget < 8.8)
		{
			printf("Box moved");
		}
	}*/
	return rewardBoxRobot1 + rewardBoxRobot2 + rewardBoxTarget;
}

double CMoveBox2RobotsReward::getMin()
{
	return 0.0;
}

double CMoveBox2RobotsReward::getMax()
{
	return 14.0;
}