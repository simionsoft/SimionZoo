#include "stdafx.h"
#include "world-box1robot.h"
#include "app.h"
#include "noise.h"
#include "Robot.h"
#include "Box.h"
#include "BulletBody.h"

double static getDistanceBetweenPoints(double x1, double y1, double x2, double y2) {
	double distance = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
	return distance;
}

#define TargetX 12.4
#define TargetY .0

#define robotOrigin_x 3.0
#define robotOrigin_y 0.0

#define boxOrigin_x 5.0
#define boxOrigin_y 0.0


CMoveBoxOneRobot::CMoveBoxOneRobot(CConfigNode* pConfigNode)
{
	METADATA("World", "MoveBoxOneRobot");

	m_rob1_X = addStateVariable("rx1", "m", -50.0, 50.0);
	m_rob1_Y = addStateVariable("ry1", "m", -50.0, 50.0);
	box_X = addStateVariable("bx", "m", -50.0, 50.0);
	box_Y = addStateVariable("by", "m", -50.0, 50.0);

	rob1_forceX = addActionVariable("r1forceX", "N", -20.0, 20.0);
	rob1_forceY = addActionVariable("r1forceY", "N", -20.0, 20.0);

	GRAVITY = -9.8;
	MASS_ROBOT = 0.5f;
	MASS_BOX = 1.f;
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
		btTransform groundTransform;
		groundTransform.setIdentity();
		groundTransform.setOrigin(btVector3(0, -50, 0));
		btScalar mass(MASS_GROUND);
		m_collisionShapes.push_back(groundShape);
		createRigidBody(mass, groundTransform, groundShape, btVector4(0, 0, 1, 1));
	}

	///Creating dynamic box obj1
	{

		btBoxShape* colShape = createBoxShape(btVector3(1, 1, 1));
		m_collisionShapes.push_back(colShape);
		btTransform startTransform;
		startTransform.setIdentity();
		startTransform.setOrigin(btVector3(boxOrigin_x, 0.0, boxOrigin_y));
		// if mass != 0.f object is dynamic
		btScalar	mass(MASS_BOX);
		btVector3 localInertia(0, 0, 0);
		colShape->calculateLocalInertia(mass, localInertia);
		btDefaultMotionState* boxMotionState = new btDefaultMotionState(startTransform);

		m_box = new Box(mass, boxMotionState, colShape, localInertia);
		m_dynamicsWorld->addRigidBody(m_box->getBody());

	}

	///creating a dynamic robot obj2 
	{
		
		btCollisionShape* robot1Shape = new btSphereShape(btScalar(0.5));
		m_Robot = new BulletBody(MASS_ROBOT, robotOrigin_x, 0, robotOrigin_y, robot1Shape,true);
		m_collisionShapes.push_back(robot1Shape);
		m_dynamicsWorld->addRigidBody(m_Robot->getBody());
	}

	//the reward function
	m_pRewardFunction->addRewardComponent(new CMoveBoxOneRobotReward());
	m_pRewardFunction->initialize();
}

void CMoveBoxOneRobot::reset(CState *s)
{
	btTransform robotTransform;
	btTransform boxTransform;

//	if (CSimionApp::get()->pExperiment->isEvaluationEpisode())
	{
		//fixed setting in evaluation episodes

		/// New update due to correct the reset
		m_Robot->getBody()->clearForces();
		btVector3 zeroVector(0, 0, 0);
		m_Robot->getBody()->setLinearVelocity(zeroVector);
		m_Robot->getBody()->setAngularVelocity(zeroVector);


		m_Robot->getBody()->getMotionState()->getWorldTransform(m_Robot->getTransform());
		m_box->getBody()->getMotionState()->getWorldTransform(boxTransform);

		/// reset robot
		m_Robot->getTransform().setOrigin(btVector3(robotOrigin_x, 0.0, robotOrigin_y));
		m_Robot->getBody()->setWorldTransform(m_Robot->getTransform());
		m_Robot->getBody()->getMotionState()->setWorldTransform(m_Robot->getTransform());

		///reset box
		boxTransform.setOrigin(btVector3(boxOrigin_x, 0.0, boxOrigin_y));
		m_box->getBody()->setWorldTransform(boxTransform);
		m_box->getBody()->getMotionState()->setWorldTransform(boxTransform);

		///set initial values to state variables
		s->set(m_rob1_X, robotOrigin_x);
		s->set(m_rob1_Y, robotOrigin_y);
		s->set(box_X, boxOrigin_x);
		s->set(box_Y, boxOrigin_y);
	}
	//else
	//{
	//	//random setting in training episodes
	//	robotTransform.setOrigin(btVector3(robotOrigin_x + getRandomValue()*0.4, 0.0, robotOrigin_y + getRandomValue()*0.4));
	//	boxTransform.setOrigin(btVector3(boxOrigin_x + getRandomValue()*0.4, 0.0, boxOrigin_y + getRandomValue()*0.2));
	//	s->set(m_rob1_X, robotOrigin_x + getRandomValue()*0.4);
	//	s->set(m_rob1_Y, robotOrigin_y + getRandomValue()*0.2);
	//	s->set(box_X, boxOrigin_x + getRandomValue()*0.4);
	//	s->set(box_Y, boxOrigin_y + getRandomValue()*0.2);
	//}
}

void CMoveBoxOneRobot::executeAction(CState *s, const CAction *a, double dt)
{

	double rob1forcex = a->get("r1forceX");
	double rob1forcey = a->get("r1forceY");

	btTransform box_trans;
	btTransform r1_trans;

	//Update Box
	m_box->getBody()->getMotionState()->getWorldTransform(box_trans);

	s->set(box_X, float(box_trans.getOrigin().getX()));
	s->set(box_Y, float(box_trans.getOrigin().getZ()));

	//Update Robot1
	m_Robot->getBody()->applyCentralForce(btVector3(rob1forcex, 0, rob1forcey));

	m_dynamicsWorld->stepSimulation(dt);

	int numCollision = m_dynamicsWorld->getDispatcher()->getNumManifolds();

	m_Robot->getBody()->getMotionState()->getWorldTransform(r1_trans);
	s->set(m_rob1_X, double(r1_trans.getOrigin().getX()));
	s->set(m_rob1_Y, double(r1_trans.getOrigin().getZ()));
	double rob1x = s->get(m_rob1_X);
	double rob1y = s->get(m_rob1_Y);

}

double CMoveBoxOneRobotReward::getReward(const CState* s, const CAction* a, const CState* s_p)
{
	CExperiment* pExperiment = CSimionApp::get()->pExperiment.ptr();
	bool bEval = CSimionApp::get()->pExperiment->isEvaluationEpisode();
	int step = CSimionApp::get()->pExperiment->getStep();

	double boxAfterX = s_p->get("bx");
	double boxAfterY = s_p->get("by");

	double robotAfterX = s_p->get("rx1");
	double robotAfterY = s_p->get("ry1");


	double distance = getDistanceBetweenPoints(TargetX, TargetY, robotAfterX, robotAfterY);

	if (robotAfterX == 50.0 || robotAfterX == -50.0 || robotAfterY == 50.0 || robotAfterY == -50.0)
	{
		/*if(robotAfterX == 50.0)
		printf("me he caido hacia adelante\n");
		if(robotAfterX == -50.0)
		printf("me he caido hacia atras\n");
		if(robotAfterY == 50.0)
		printf("me he caido hacia la derecha\n");
		if(robotAfterY == -50.0)
		printf("me he caido hacia la izquierda\n");*/
		CSimionApp::get()->pExperiment->setTerminalState();
		return -1;
	}
	if (distance < 0.5)
	{
		CSimionApp::get()->pExperiment->setTerminalState();
		//printf("llego\n");
		return 1;
	}
	return 1 /( distance*distance);
}

double CMoveBoxOneRobotReward::getMin()
{
	return -1.0;
}

double CMoveBoxOneRobotReward::getMax()
{
	return 1.0;
}