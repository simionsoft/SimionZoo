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


OpenGLGuiHelper *gui;
CommonExampleOptions *options;

COnlyRobot::COnlyRobot(CConfigNode* pConfigNode)
{

	METADATA("World", "MoveRobotOnly");

	rob1_X = addStateVariable("rx1", "m", -50.0, 50.0);
	rob1_Y = addStateVariable("ry1", "m", -50.0, 50.0);

	rob1_forceX = addActionVariable("r1forceX", "N", -20.0, 20.0);
	rob1_forceY = addActionVariable("r1forceY", "N", -20.0, 20.0);

	MASS_ROBOT = 0.5f;
	MASS_GROUND = 0.f;


	app = new SimpleOpenGL3App("Graphic Example", 1024, 768, true);
	///Graphic init
	gui= new OpenGLGuiHelper(app, false);
	options= new CommonExampleOptions(gui);

	robotOnlyGraphs = new GraphicSettings(options->m_guiHelper);
	robotOnlyGraphs->initializeGraphicProccess();
	robotOnlyGraphs->setOpenGLApp(app);
	
	
	///Creating static object, ground
	{
		btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(50.), btScalar(50.), btScalar(50.)));
		ground_bb = new BulletBody(MASS_GROUND, ground_x, ground_y, ground_z, groundShape, false);
		robotOnlyGraphs->getCollisionShape().push_back(ground_bb->getShape());
		robotOnlyGraphs->getDynamicsWorld()->addRigidBody(ground_bb->getBody());
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

	//	if (CSimionApp::get()->pExperiment->isEvaluationEpisode())
	{
		//fixed setting in evaluation episodes

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
	}
	//else
	//{
	//	//random setting in training episodes
	//	robotTransform.setOrigin(btVector3(robotOrigin_x + getRandomValue()*0.4, 0.0, robotOrigin_y + getRandomValue()*0.4));
	//	boxTransform.setOrigin(btVector3(boxOrigin_x + getRandomValue()*0.4, 0.0, boxOrigin_y + getRandomValue()*0.2));
	//	s->set(rob1_X, robotOrigin_x + getRandomValue()*0.4);
	//	s->set(rob1_Y, robotOrigin_y + getRandomValue()*0.2);
	//	s->set(box_X, boxOrigin_x + getRandomValue()*0.4);
	//	s->set(box_Y, boxOrigin_y + getRandomValue()*0.2);
	//}
}

void COnlyRobot::executeAction(CState *s, const CAction *a, double dt)
{

	double rob1forcex = a->get("r1forceX");
	double rob1forcey = a->get("r1forceY");

	btTransform r1_trans;

	//Update Robot1
	robot_bb->getBody()->applyCentralForce(btVector3(rob1forcex, 0, rob1forcey));

	robotOnlyGraphs->simulate(dt);
	if (CSimionApp::get()->pExperiment->isEvaluationEpisode())
		robotOnlyGraphs->draw();


	robot_bb->getBody()->getMotionState()->getWorldTransform(r1_trans);
	s->set(rob1_X, double(r1_trans.getOrigin().getX()));
	s->set(rob1_Y, double(r1_trans.getOrigin().getZ()));
	double rob1x = s->get(rob1_X);
	double rob1y = s->get(rob1_Y);

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
	if (distance < 0.5)
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

