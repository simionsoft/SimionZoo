#include "stdafx.h"
#include "world-box1robot.h"
#include "app.h"
#include "../3rd-party/bullet3-2.86/src/btBulletDynamicsCommon.h"
#include "noise.h"
#include "Robot.h"
#include "Box.h"

Robot *m_pRobot1;
Robot *m_pRobot2;
Box *m_box;

CMoveBoxOneRobot::CMoveBoxOneRobot(CConfigNode* pConfigNode)
{
	METADATA("World", "MoveBoxOneRobot");


	rob1_X = addStateVariable("rx1", "m", -50.0, 50.0);
	rob1_Y = addStateVariable("ry1", "m", -50.0, 50.0);
	box_X = addStateVariable("bx", "m", -50.0, 50.0);
	box_Y = addStateVariable("by", "m", -50.0, 50.0);
	addConstant("TargetPointX", getRandomForTarget());
	addConstant("TargetPointY", getRandomForTarget());

	rob1_forceX = addActionVariable("r1forceX", "N", -8.0, 8.0);
	rob1_forceY = addActionVariable("r1forceY", "N", -8.0, 8.0);

	GRAVITY = -9.8;
	MASS_ROBOT = 1.f;
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
		m_collisionShapes.push_back(groundShape);
		btTransform groundTransform;
		groundTransform.setIdentity();
		groundTransform.setOrigin(btVector3(0, -50, 0));
		btScalar mass(MASS_GROUND);
		createRigidBody(mass, groundTransform, groundShape, btVector4(0, 0, 1, 1));
	}

	///Creating dynamic box obj1
	{

		btBoxShape* colShape = createBoxShape(btVector3(1, 1, 1));
		m_collisionShapes.push_back(colShape);
		btTransform startTransform;
		startTransform.setIdentity();
		startTransform.setOrigin(btVector3(3.0, 0.0, 0.0));
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
		btCollisionShape* robot1Shape = new btSphereShape(btScalar(1));
		m_collisionShapes.push_back(robot1Shape);
		btTransform robot1startTransform;
		robot1startTransform.setIdentity();
		robot1startTransform.setOrigin(btVector3(0.5, 0.0, 0.0));
		// if mass != 0.f object is dynamic
		btScalar robot1mass(MASS_ROBOT);
		btVector3 robot1localInertia(0, 0, 0);
		robot1Shape->calculateLocalInertia(robot1mass, robot1localInertia);
		btDefaultMotionState* robot1MotionState = new btDefaultMotionState(robot1startTransform);

		m_pRobot1 = new Robot(robot1mass, robot1MotionState, robot1Shape, robot1localInertia);
		m_dynamicsWorld->addRigidBody(m_pRobot1->getBody());
	}

	//the reward function
	m_pRewardFunction->addRewardComponent(new CMoveBoxOneRobotReward());
	m_pRewardFunction->initialize();
}

void CMoveBoxOneRobot::reset(CState *s)
{

	if (CSimionApp::get()->pExperiment->isEvaluationEpisode())
	{
		//fixed setting in evaluation episodes
		s->set(rob1_X, 0.5);
		s->set(rob1_Y, 0.0);
		s->set(box_X, 3.0);
		s->set(box_Y, 0.0);
		//faltan theta

	}
	else
	{
		//random setting in training episodes

		s->set(rob1_X, 0.2 + getRandomValue()*0.4);
		s->set(rob1_Y, 0.7 + getRandomValue()*0.2);
		s->set(box_X, 2.5 + getRandomValue()*0.4);
		s->set(box_Y, 0.0 + getRandomValue()*0.2);
		//faltan theta
	}
}

void CMoveBoxOneRobot::executeAction(CState *s, const CAction *a, double dt)
{

	double rob1force_x = a->get(rob1_forceX);
	double rob1force_y = a->get(rob1force_y);

	btVector3 applied_force1;

	m_dynamicsWorld->stepSimulation(dt);

	btTransform box_trans;
	btTransform r1_trans;

	//Update Box
	m_box->getBody()->getMotionState()->getWorldTransform(box_trans);
	s->set(box_X, float(box_trans.getOrigin().getX()));
	s->set(box_Y, float(box_trans.getOrigin().getZ()));

	//Update Robot1
	m_pRobot1->setAppliedForce(btVector3(rob1force_x, 0, rob1force_y));
	m_pRobot1->getAppliedForce(applied_force1);
	m_pRobot1->getBody()->applyCentralForce(applied_force1);
	m_pRobot1->getBody()->getMotionState()->getWorldTransform(r1_trans);
	s->set(rob1_X, float(r1_trans.getOrigin().getX()));
	s->set(rob1_Y, float(r1_trans.getOrigin().getZ()));

}

double CMoveBoxOneRobotReward::getReward(const CState* s, const CAction* a, const CState* s_p)
{
	bool bEval = CSimionApp::get()->pExperiment->isEvaluationEpisode();
	int step = CSimionApp::get()->pExperiment->getStep();

	// get goal coordinates (constant)
	double tX = s->get("TargetPointX");
	double tY = s->get("TargetPointY");

	double boxAfterX = s_p->get("bx");
	double boxAfterY = s_p->get("by");

	double distance = getDistanceBetweenPoints(tX, tY, boxAfterX, boxAfterY);
	if (distance < 2) {
		return 0.9;
	}
	if (distance < 0.5) {
		CSimionApp::get()->pExperiment->setTerminalState();
		return 1;
	}
	return 2 / distance;
}

double CMoveBoxOneRobotReward::getMin()
{
	return 0.0;
}

double CMoveBoxOneRobotReward::getMax()
{
	return 1.0;
}

double getRandomForTarget()
{
	return (double)(rand() % 50);
}

double getDistanceBetweenPoints(double x1, double y1, double x2, double y2) {
	double distance = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
	return distance;
}