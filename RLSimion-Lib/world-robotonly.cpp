#include "stdafx.h"
#include "world-robotonly.h"
#include "app.h"
#include "noise.h"
#include "Robot.h"


double static getDistanceBetweenPoints(double x1, double y1, double x2, double y2) {
	double distance = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
	return distance;
}

#define TargetX 8.0
#define TargetY 2.0

#define robotOrigin_x 3.0
#define robotOrigin_y 0.0


COnlyRobot::COnlyRobot(CConfigNode* pConfigNode)
{
	METADATA("World", "MoveRobotOnly");

	rob1_X = addStateVariable("rx1", "m", -50.0, 50.0);
	rob1_Y = addStateVariable("ry1", "m", -50.0, 50.0);

	rob1_forceX = addActionVariable("r1forceX", "N", -20.0, 20.0);
	rob1_forceY = addActionVariable("r1forceY", "N", -20.0, 20.0);

	GRAVITY = -9.8;
	MASS_ROBOT = 0.5f;
	MASS_GROUND = 0.f;

	m_collisionConfiguration = new btDefaultCollisionConfiguration();
	m_dispatcher = new	btCollisionDispatcher(m_collisionConfiguration);
	m_broadphase = new btDbvtBroadphase();
	btSequentialImpulseConstraintSolver* sol = new btSequentialImpulseConstraintSolver;
	m_solver = sol;
	m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_solver, m_collisionConfiguration);
	m_dynamicsWorld->setGravity(btVector3(0, GRAVITY, 0));
	btAlignedObjectArray<btCollisionShape*>	m_collisionShapes;

	///Creating static object, ground
	{
		btBoxShape* groundShape = createBoxShape(btVector3(btScalar(50.), btScalar(50.), btScalar(50.)));
		m_collisionShapes.push_back(groundShape);
		btTransform groundTransform;
		groundTransform.setIdentity();
		groundTransform.setOrigin(btVector3(0, -50, 0));
		btScalar mass(MASS_GROUND);
		createRigidBody(mass, groundTransform, groundShape, btVector4(0, 0, 1, 1));
	}

	///creating a dynamic robot obj1
	{
		btCollisionShape* robot1Shape = new btSphereShape(btScalar(0.5));
		m_collisionShapes.push_back(robot1Shape);
		btTransform robot1startTransform;
		robot1startTransform.setIdentity();
		robot1startTransform.setOrigin(btVector3(robotOrigin_x, 0.0, robotOrigin_y));
		// if mass != 0.f object is dynamic
		btScalar robot1mass(MASS_ROBOT);
		btVector3 robot1localInertia(0, 0, 0);
		robot1Shape->calculateLocalInertia(robot1mass, robot1localInertia);
		btDefaultMotionState* robot1MotionState = new btDefaultMotionState(robot1startTransform);

		m_pRobot1 = new Robot(robot1mass, robot1MotionState, robot1Shape, robot1localInertia);
		m_dynamicsWorld->addRigidBody(m_pRobot1->getBody());
		m_pRobot1->getBody()->setActivationState(DISABLE_DEACTIVATION);
	}

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
		m_pRobot1->getBody()->clearForces();
		btVector3 zeroVector(0, 0, 0);
		m_pRobot1->getBody()->setLinearVelocity(zeroVector);
		m_pRobot1->getBody()->setAngularVelocity(zeroVector);


		m_pRobot1->getBody()->getMotionState()->getWorldTransform(robotTransform);

		/// reset robot
		robotTransform.setOrigin(btVector3(robotOrigin_x, 0.0, robotOrigin_y));
		m_pRobot1->getBody()->setWorldTransform(robotTransform);
		m_pRobot1->getBody()->getMotionState()->setWorldTransform(robotTransform);
	
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
	m_pRobot1->getBody()->applyCentralForce(btVector3(rob1forcex, 0, rob1forcey));
	m_dynamicsWorld->stepSimulation(dt);
	m_pRobot1->getBody()->getMotionState()->getWorldTransform(r1_trans);
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
		//if(robotAfterX == 50.0)
		//printf("me he caido hacia adelante\n");
		//if(robotAfterX == -50.0)
		//printf("me he caido hacia atras\n");
		//if(robotAfterY == 50.0)
		//printf("me he caido hacia la derecha\n");
		//if(robotAfterY == -50.0)
		//printf("me he caido hacia la izquierda\n");
		CSimionApp::get()->pExperiment->setTerminalState();
		return -1;
	}
	if (distance < 0.5)
	{
		CSimionApp::get()->pExperiment->setTerminalState();
		//printf("llego\n");
		//return 1;
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