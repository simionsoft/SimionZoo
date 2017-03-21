#pragma once

#include "world.h"
#include "reward.h"
#define ATTRIBUTE_ALIGNED16(a) a
#include "../3rd-party/bullet3-2.86/src/btBulletDynamicsCommon.h"
class btCollisionShape;
class Robot;
class Box;


//Move box with 2 robots
class CMoveBox2Robots : public CDynamicModel
{
	/// All-Simulation variables
	double GRAVITY;
	double MASS_ROBOT;
	double MASS_BOX;
	double MASS_GROUND;

	/// Episode variables
	double rob1_X, rob1_Y;
	double rob2_X, rob2_Y;
	double box_X, box_Y;
	

	// Action variables
	double rob1_forceX, rob1_forceY;
	double rob2_forceX, rob2_forceY;	

	///inicialización
	btAlignedObjectArray<btCollisionShape*>	m_collisionShapes;
	btBroadphaseInterface*	m_broadphase;
	btCollisionDispatcher*	m_dispatcher;
	btConstraintSolver*	m_solver;
	btDefaultCollisionConfiguration* m_collisionConfiguration;
	btDiscreteDynamicsWorld* m_dynamicsWorld;

	Robot *m_pRobot1;
	Robot *m_pRobot2;
	Box *m_box;
public:
	CMoveBox2Robots(CConfigNode* pParameters);
	virtual ~CMoveBox2Robots() = default;

	void reset(CState *s);
	void executeAction(CState *s, const CAction *a, double dt);

	btBoxShape* createBoxShape(const btVector3& halfExtents)
	{
		btBoxShape* box = new btBoxShape(halfExtents);
		return box;
	}

	btRigidBody*	createRigidBody(float mass, const btTransform& startTransform, btCollisionShape* shape, const btVector4& color = btVector4(1, 0, 0, 1))
	{
		
		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			shape->calculateLocalInertia(mass, localInertia);

		//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects

#define USE_MOTIONSTATE 1
#ifdef USE_MOTIONSTATE
		btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);

		btRigidBody::btRigidBodyConstructionInfo cInfo(mass, myMotionState, shape, localInertia);

		btRigidBody* body = new btRigidBody(cInfo);

#else
		btRigidBody* body = new btRigidBody(mass, 0, shape, localInertia);
		body->setWorldTransform(startTransform);
#endif//

		body->setUserIndex(-1);
		m_dynamicsWorld->addRigidBody(body);
		return body;
	}
};

class CMoveBox2RobotsReward : public IRewardComponent
{
public:
	double getReward(const CState *s, const CAction *a, const CState *s_p);
	const char* getName() { return "reward"; }
	double getMin();
	double getMax();
};