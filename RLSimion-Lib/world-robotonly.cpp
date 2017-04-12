#include "stdafx.h"
#include "world-robotonly.h"
#include "app.h"
#include "noise.h"
#include "Robot.h"
#include "GraphicSettings.h"
#include "BulletBody.h"
#pragma comment(lib,"opengl32.lib")

#define GLEW_STATIC

double static getDistanceBetweenPoints(double x1, double y1, double x2, double y2) {
	double distance = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
	return distance;
}

#define TargetX 8.0
#define TargetY 2.0

#define robotOrigin_x 3.0
#define robotOrigin_y 0.0

#define ground_x 0.0
#define ground_y -50.0
#define ground_z 0.0 

#define theta_o 0.0

#define CNT_VEL 2.0

OpenGLGuiHelper *gui;
CommonExampleOptions *options;

COnlyRobot::COnlyRobot(CConfigNode* pConfigNode)
{

	METADATA("World", "MoveRobotOnly");

	rob1_X = addStateVariable("rx1", "m", -50.0, 50.0);
	rob1_Y = addStateVariable("ry1", "m", -50.0, 50.0);
	m_theta = addStateVariable("theta", "rad", -3.14, 3.14);

	m_linear_vel = addActionVariable("v", "m/s", -2.0, 2.0);
	rob1_VelX = CNT_VEL;
	rob1_VelY = CNT_VEL;
	m_omega = addActionVariable("omega", "rad", -3.0, 3.0);

	MASS_ROBOT = 0.5f;
	MASS_GROUND = 0.f;
	MASS_TARGET = 0.1f;


	app = new SimpleOpenGL3App("Graphic Bullet Interface", 1024, 768, true);
	///Graphic init
	gui= new OpenGLGuiHelper(app, false);
	options= new CommonExampleOptions(gui);

	robotOnlyGraphs = new GraphicSettings(options->m_guiHelper);
	robotOnlyGraphs->initializeGraphicProccess();
	robotOnlyGraphs->setOpenGLApp(app);
	
	
	///Creating static object, ground
	{
		btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(50.), btScalar(50.), btScalar(50.)));
		// "false" parameter will notify the constructor not to disable activation state
		ground_bb = new BulletBody(MASS_GROUND, ground_x, ground_y, ground_z, groundShape, false);
		robotOnlyGraphs->getCollisionShape().push_back(ground_bb->getShape());
		robotOnlyGraphs->getDynamicsWorld()->addRigidBody(ground_bb->getBody());
	}

	/// Creating target point, static
	{
		btCollisionShape* targetShape = new btConeShape(btScalar(0.5), btScalar(5.5));
		target_bb = new BulletBody(MASS_TARGET, TargetX, 0, TargetY+1, targetShape, false);
		target_bb->getBody()->setCollisionFlags(2);
		robotOnlyGraphs->getCollisionShape().push_back(target_bb->getShape());
		robotOnlyGraphs->getDynamicsWorld()->addRigidBody(target_bb->getBody());
	}

	///creating a dynamic robot obj1
	{
		//btCollisionShape* robot1Shape = new btSphereShape(btScalar(0.5));
		robot_bb = new BulletBody(MASS_ROBOT, robotOrigin_x, 0, robotOrigin_y, new btSphereShape(btScalar(0.5)), true);
		robotOnlyGraphs->getCollisionShape().push_back(robot_bb->getShape());
		robotOnlyGraphs->getDynamicsWorld()->addRigidBody(robot_bb->getBody());
	}

	///Graphic init
	robotOnlyGraphs->generateGraphics(robotOnlyGraphs->getGuiHelper());

	//the reward function
	m_pRewardFunction->addRewardComponent(new COnlyRobotReward());
	m_pRewardFunction->initialize();
}

void COnlyRobot::reset(CState *s)
{
	btTransform robotTransform;

	{
		/// New update due to correct the reset
		robot_bb->getBody()->clearForces();
		btVector3 zeroVector(0, 0, 0);
		robot_bb->getBody()->setLinearVelocity(zeroVector);
		robot_bb->getBody()->setAngularVelocity(zeroVector);


		robot_bb->getBody()->getMotionState()->getWorldTransform(robotTransform);

		/// reset robot
		robotTransform.setOrigin(btVector3(robotOrigin_x, 0.0, robotOrigin_y));
		robot_bb->getBody()->setWorldTransform(robotTransform);
		robot_bb->getBody()->getMotionState()->setWorldTransform(robotTransform);
	
		///set initial values to state variables
		s->set(rob1_X, robotOrigin_x);
		s->set(rob1_Y, robotOrigin_y);
		s->set(m_theta, theta_o);
	}
}

void COnlyRobot::executeAction(CState *s, const CAction *a, double dt)
{

	double omega = a->get("omega");
	double theta = s->get(m_theta);
	double linear_vel = a->get("v");

	theta = theta + omega*dt;

	if (theta > SIMD_2_PI)
		theta -= SIMD_2_PI;

	btTransform r1_trans;

	rob1_VelX = cos(theta)*linear_vel;
	rob1_VelY = sin(theta)*linear_vel;

	robot_bb->getBody()->setAngularVelocity(btVector3(0.0, omega, 0.0));
	robot_bb->getBody()->setLinearVelocity(btVector3(rob1_VelX, 0.0, rob1_VelY));

	//Update Robot1
	robotOnlyGraphs->simulate(dt);

	robot_bb->getBody()->getMotionState()->getWorldTransform(r1_trans);
	btVector3 printPosition = btVector3(r1_trans.getOrigin().getX(), r1_trans.getOrigin().getY() + 5, r1_trans.getOrigin().getZ());
	if (CSimionApp::get()->pExperiment->isEvaluationEpisode())
	{
		robotOnlyGraphs->drawText3D("Evaluation episode",printPosition);
		
	}
	else
	{
		robotOnlyGraphs->drawText3D("Training episode", printPosition);
	}
	robotOnlyGraphs->draw();
	
	s->set(rob1_X, double(r1_trans.getOrigin().getX()));
	s->set(rob1_Y, double(r1_trans.getOrigin().getZ()));
	s->set(m_theta, theta);

}

double COnlyRobotReward::getReward(const CState* s, const CAction* a, const CState* s_p)
{
	CExperiment* pExperiment = CSimionApp::get()->pExperiment.ptr();
	bool bEval = CSimionApp::get()->pExperiment->isEvaluationEpisode();
	int step = CSimionApp::get()->pExperiment->getStep();

	double robotAfterX = s_p->get("rx1");
	double robotAfterY = s_p->get("ry1");

	double distance = getDistanceBetweenPoints(TargetX, TargetY, robotAfterX, robotAfterY);

	if (robotAfterX >= 50.0 || robotAfterX <= -50.0 || robotAfterY >= 50.0 || robotAfterY <= -50.0)
	{
		CSimionApp::get()->pExperiment->setTerminalState();
		return -1;
	}
	if (distance < 0.25)
	{
		CSimionApp::get()->pExperiment->setTerminalState();
	}
	distance = std::max(distance, 0.0001);
	return 1 / (distance);
}

double COnlyRobotReward::getMin()
{
	return -1.0;
}

double COnlyRobotReward::getMax()
{
	return 1.0;
}

COnlyRobot::~COnlyRobot()
{
	delete options;
	delete gui;
}

