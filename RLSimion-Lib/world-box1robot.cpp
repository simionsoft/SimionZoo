#include "stdafx.h"
#include "world-box1robot.h"
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

#define TargetX 10.0
#define TargetY 3.0

#define ground_x 0.0
#define ground_y -50.0
#define ground_z 0.0 

#define robotOrigin_x 0.0
#define robotOrigin_y 0.0

#define boxOrigin_x 3.0
#define boxOrigin_y 2.0

#define theta_o 0.0

OpenGLGuiHelper *guiHelper;
CommonExampleOptions *opt;

CMoveBoxOneRobot::CMoveBoxOneRobot(CConfigNode* pConfigNode)
{
	METADATA("World", "MoveBoxOneRobot");

	m_rob1_X = addStateVariable("rx1", "m", -20.0, 20.0);
	m_rob1_Y = addStateVariable("ry1", "m", -20.0, 20.0);
	m_box_X = addStateVariable("bx", "m", -20.0, 20.0);
	m_box_Y = addStateVariable("by", "m", -20.0, 20.0);

	m_D_BrX = addStateVariable("dBrX", "m", -20.0, 20.0);
	m_D_BrY = addStateVariable("dBrY", "m", -20.0, 20.0);
	m_D_BtX = addStateVariable("dBtX", "m", -20.0, 20.0);
	m_D_BtY = addStateVariable("dBtY", "m", -20.0, 20.0);
	m_theta = addStateVariable("theta", "rad", -3.15, 3.15);

	m_linear_vel = addActionVariable("v", "m/s", -2.0, 2.0);
	m_omega = addActionVariable("omega", "rad/s", -8.0, 8.0);

	MASS_ROBOT = 1.5f;
	MASS_BOX = 1.f;
	MASS_GROUND = 0.f;
	MASS_TARGET = 0.1f;

	window = new SimpleOpenGL3App("Graphic Bullet One Robot Interface", 1024, 768, true);

	///Graphic init
	guiHelper = new OpenGLGuiHelper(window, false);
	opt = new CommonExampleOptions(guiHelper);

	rBoxBuilder = new BulletBuilder(opt->m_guiHelper);
	rBoxBuilder->initializeBulletRequirements();
	rBoxBuilder->setOpenGLApp(window);

	///Creating static object, ground
	{
		m_Ground = new BulletBody(MASS_GROUND, ground_x, ground_y, ground_z, new btBoxShape(btVector3(btScalar(50.), btScalar(50.), btScalar(50.))), false);
		rBoxBuilder->getCollisionShape().push_back(m_Ground->getShape());
		rBoxBuilder->getDynamicsWorld()->addRigidBody(m_Ground->getBody());
	}

	/// Creating target point, static
	{
		m_Target = new BulletBody(MASS_TARGET, TargetX, 5, TargetY + 1, new btConeShape(btScalar(0.5), btScalar(0.001)), false);
		m_Target->getBody()->setCollisionFlags(m_Target->getBody()->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		rBoxBuilder->getCollisionShape().push_back(m_Target->getShape());
		rBoxBuilder->getDynamicsWorld()->addRigidBody(m_Target->getBody());
	}

	///Creating dynamic box
	{
		m_Box = new BulletBody(MASS_BOX, boxOrigin_x, 0, boxOrigin_y, new btBoxShape(btVector3(btScalar(0.6), btScalar(0.6), btScalar(0.6))), true);
		rBoxBuilder->getCollisionShape().push_back(m_Box->getShape());
		rBoxBuilder->getDynamicsWorld()->addRigidBody(m_Box->getBody());
	}

	///creating a dynamic robot  
	{
		m_Robot = new BulletBody(MASS_ROBOT, robotOrigin_x, 0, robotOrigin_y, new btSphereShape(btScalar(0.5)), true);
		rBoxBuilder->getCollisionShape().push_back(m_Robot->getShape());
		rBoxBuilder->getDynamicsWorld()->addRigidBody(m_Robot->getBody());
	}

	o_distBrX = getDistanceOneDimension(robotOrigin_x, boxOrigin_x);
	o_distBrY = getDistanceOneDimension(robotOrigin_y, boxOrigin_y);
	o_distBtX = getDistanceOneDimension(robotOrigin_x, TargetX);
	o_distBtY = getDistanceOneDimension(robotOrigin_y, TargetY);

	///Graphic init
	rBoxBuilder->generateGraphics(rBoxBuilder->getGuiHelper());

	//the reward function
	m_pRewardFunction->addRewardComponent(new CMoveBoxOneRobotReward());
	m_pRewardFunction->initialize();
}

void CMoveBoxOneRobot::reset(CState *s)
{
	btTransform robotTransform;
	btTransform boxTransform;
	btTransform targetTransform;
	btQuaternion orientation = {0.000000000, 0.000000000, 0.000000000, 1.00000000};


	{
		m_Robot->getBody()->clearForces();
		btVector3 zeroVector(0, 0, 0);
		m_Robot->getBody()->setLinearVelocity(zeroVector);
		m_Robot->getBody()->setAngularVelocity(zeroVector);


		m_Robot->getBody()->getMotionState()->getWorldTransform(robotTransform);
		m_Box->getBody()->getMotionState()->getWorldTransform(boxTransform);
		m_Target->getBody()->getMotionState()->getWorldTransform(targetTransform);

		/// reset robot
		robotTransform.setOrigin(btVector3(robotOrigin_x, 0.0, robotOrigin_y));
		m_Robot->getBody()->setWorldTransform(robotTransform);
		m_Robot->getBody()->getMotionState()->setWorldTransform(robotTransform);

		///reset box
		boxTransform.setOrigin(btVector3(boxOrigin_x, 0.0, boxOrigin_y));
		boxTransform.setRotation(orientation);
		m_Box->getBody()->setWorldTransform(boxTransform);
		m_Box->getBody()->getMotionState()->setWorldTransform(boxTransform);

		/////reset target (maybe not necessary)
		targetTransform.setOrigin(btVector3(TargetX, 0.0, TargetY));
		m_Target->getBody()->setWorldTransform(targetTransform);
		m_Target->getBody()->getMotionState()->setWorldTransform(targetTransform);

		///set initial values to state variables

		s->set(m_D_BrX, o_distBrX);
		s->set(m_D_BrY, o_distBrY);
		s->set(m_D_BtX, o_distBtX);
		s->set(m_D_BtX, o_distBtY);

		///set initial values to state variables
		s->set(m_rob1_X, robotOrigin_x);
		s->set(m_rob1_Y, robotOrigin_y);
		s->set(m_box_X, boxOrigin_x);
		s->set(m_box_Y, boxOrigin_y);
		s->set(m_theta, theta_o);

		s->set(m_theta, theta_o);
	}
}

void CMoveBoxOneRobot::executeAction(CState *s, const CAction *a, double dt)
{
	btTransform box_trans;
	btTransform rob_trans;
	double rob_VelX, rob_VelY;

	double omega = a->get("omega");
	double linear_vel = a->get("v");

	double theta = s->get(m_theta);
	theta+= omega*dt;

	if (theta > SIMD_2_PI)
		theta -= SIMD_2_PI;
	if (theta < -SIMD_2_PI)
		theta += SIMD_2_PI;

	rob_VelX = cos(theta)*linear_vel;
	rob_VelY = sin(theta)*linear_vel;

	m_Robot->getBody()->setAngularVelocity(btVector3(0.0, omega, 0.0));
    m_Robot->getBody()->setLinearVelocity(btVector3(rob_VelX, 0.0, rob_VelY));

	//Execute simulation
	rBoxBuilder->getDynamicsWorld()->stepSimulation(dt, 20);

	//Update
	{
		m_Box->getBody()->getMotionState()->getWorldTransform(box_trans);
		m_Robot->getBody()->getMotionState()->getWorldTransform(rob_trans);

		s->set(m_box_X, float(box_trans.getOrigin().getX()));
		s->set(m_box_Y, float(box_trans.getOrigin().getZ()));

		s->set(m_rob1_X, double(rob_trans.getOrigin().getX()));
		s->set(m_rob1_Y, double(rob_trans.getOrigin().getZ()));

		s->set(m_D_BrX, getDistanceOneDimension(rob_trans.getOrigin().getX(), box_trans.getOrigin().getX()));
		s->set(m_D_BrY, getDistanceOneDimension(rob_trans.getOrigin().getY(), box_trans.getOrigin().getY()));

		s->set(m_D_BtX, getDistanceOneDimension(TargetX, box_trans.getOrigin().getX()));
		s->set(m_D_BtY, getDistanceOneDimension(TargetY, box_trans.getOrigin().getX()));

		s->set(m_theta, theta);
	}

	//draw
	int numManifolds = rBoxBuilder->getDynamicsWorld()->getDispatcher()->getNumManifolds();
	char num[10];
	sprintf(num, "%d", numManifolds);
	btVector3 printPosition = btVector3(rob_trans.getOrigin().getX(), rob_trans.getOrigin().getY() + 5, rob_trans.getOrigin().getZ());
	btVector3 printManifoldsPosition = btVector3(rob_trans.getOrigin().getX(), rob_trans.getOrigin().getY() + 8, rob_trans.getOrigin().getZ());
	if (CSimionApp::get()->pExperiment->isEvaluationEpisode())
	{
		rBoxBuilder->drawText3D("Evaluation episode", printPosition);
		rBoxBuilder->drawText3D(num , printManifoldsPosition);
	}
	else
	{
		rBoxBuilder->drawText3D("Training episode", printPosition);
		rBoxBuilder->drawText3D(num, printManifoldsPosition);
	}
	if (!CSimionApp::get()->isExecutedRemotely()) {
		rBoxBuilder->draw();
	}

}

double CMoveBoxOneRobotReward::getReward(const CState* s, const CAction* a, const CState* s_p)
{
	double boxAfterX = s_p->get("bx");
	double boxAfterY = s_p->get("by");

	double robotAfterX = s_p->get("rx1");
	double robotAfterY = s_p->get("ry1");


	double distanceRob = getDistanceBetweenPoints(TargetX, TargetY, robotAfterX, robotAfterY);
	double distance = getDistanceBetweenPoints(robotAfterX, robotAfterY, boxAfterX, boxAfterY);

	if (robotAfterX >= 50.0 || robotAfterX <= -50.0 || robotAfterY >= 50.0 || robotAfterY <= -50.0)
	{
		CSimionApp::get()->pExperiment->setTerminalState();
		return -1;
	}
	distance = std::max(distance, 0.0001);
	distanceRob = std::max(distanceRob, 0.0001);
	return ((1 / distanceRob) + (2 / (distance)));
}

double CMoveBoxOneRobotReward::getMin()
{
	return -1.0;
}

double CMoveBoxOneRobotReward::getMax()
{
	return 3.0;
}

CMoveBoxOneRobot::~CMoveBoxOneRobot()
{
	delete opt;
	delete guiHelper;
}