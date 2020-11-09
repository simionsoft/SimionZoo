#pragma once

#include "../../../3rd-party/bullet3-2.86/src/btBulletDynamicsCommon.h"
#include "../../../3rd-party/bullet3-2.86/src/BulletSoftBody/btSoftRigidDynamicsWorld.h"
#include "../../../3rd-party/bullet3-2.86/src/BulletSoftBody/btSoftBodyHelpers.h"
#include "../../../3rd-party/bullet3-2.86/src/BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h"

struct BulletCreationInterface
{
	///inicializaci�n
	btAlignedObjectArray<btCollisionShape*>	m_collisionShapes;
	btBroadphaseInterface*	m_broadphase;
	btCollisionDispatcher*	m_dispatcher;
	btConstraintSolver*	m_solver;
	btDefaultCollisionConfiguration* m_collisionConfiguration;
	btDiscreteDynamicsWorld* m_dynamicsWorld;
	btSoftBodyWorldInfo m_sBodyWorldInfo;
	btSoftBodyWorldInfo* s_pBodyInfo = &m_sBodyWorldInfo;

	BulletCreationInterface()
		:m_broadphase(0),
		m_dispatcher(0),
		m_solver(0),
		m_collisionConfiguration(0),
		m_dynamicsWorld(0), s_pBodyInfo{ &m_sBodyWorldInfo }
	{
	}
	virtual ~BulletCreationInterface()
	{
	}


	btDiscreteDynamicsWorld*	getDynamicsWorld()
	{
		return m_dynamicsWorld;
	}

	btSoftRigidDynamicsWorld*   getSoftDynamicsWorld()
	{
		return (btSoftRigidDynamicsWorld*)m_dynamicsWorld;
	}


	virtual void createEmptySoftWorld() {
		m_collisionConfiguration = new btSoftBodyRigidBodyCollisionConfiguration();
		m_dispatcher = new	btCollisionDispatcher(m_collisionConfiguration);
		m_broadphase = new btDbvtBroadphase();
		///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
		btSequentialImpulseConstraintSolver* sol = new btSequentialImpulseConstraintSolver;
		m_solver = sol;
		m_dynamicsWorld = new btSoftRigidDynamicsWorld(m_dispatcher, m_broadphase, m_solver, m_collisionConfiguration);
		m_dynamicsWorld->setGravity(btVector3(0.f, -9.8f, 0.f));

		getSoftBodyWorldInfo()->m_broadphase = m_broadphase;
		getSoftBodyWorldInfo()->m_dispatcher = m_dispatcher;
		getSoftBodyWorldInfo()->m_gravity = m_dynamicsWorld->getGravity();
		getSoftBodyWorldInfo()->m_sparsesdf.Initialize();
	}

	/// inicializaci�n, todo por defecto
	virtual void createEmptyDynamicsWorld()
	{
		m_collisionConfiguration = new btDefaultCollisionConfiguration();
		m_dispatcher = new	btCollisionDispatcher(m_collisionConfiguration);

		m_broadphase = new btDbvtBroadphase();

		///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
		btSequentialImpulseConstraintSolver* sol = new btSequentialImpulseConstraintSolver;
		m_solver = sol;

		m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_solver, m_collisionConfiguration);

		m_dynamicsWorld->setGravity(btVector3(0.f, -9.8f, 0.f));
	}


	virtual void stepSimulation(float deltaTime, int maxSubSteps)
	{
		if (m_dynamicsWorld)
		{
			m_dynamicsWorld->stepSimulation(deltaTime,maxSubSteps);
		}
	}

	virtual void exitPhysics()
	{
		//cleanup in the reverse order of creation/initialization

		//remove the rigidbodies from the dynamics world and delete them

		if (m_dynamicsWorld)
		{

			int i;
			for (i = m_dynamicsWorld->getNumConstraints() - 1; i >= 0; i--)
			{
				m_dynamicsWorld->removeConstraint(m_dynamicsWorld->getConstraint(i));
			}
			for (i = m_dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
			{
				btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];
				btRigidBody* body = btRigidBody::upcast(obj);
				if (body && body->getMotionState())
				{
					delete body->getMotionState();
				}
				m_dynamicsWorld->removeCollisionObject(obj);
				delete obj;
			}
		}
		//delete collision shapes
		for (int j = 0; j<m_collisionShapes.size(); j++)
		{
			btCollisionShape* shape = m_collisionShapes[j];
			delete shape;
		}
		m_collisionShapes.clear();

		delete m_dynamicsWorld;
		m_dynamicsWorld = 0;

		delete m_solver;
		m_solver = 0;

		delete m_broadphase;
		m_broadphase = 0;

		delete m_dispatcher;
		m_dispatcher = 0;

		delete m_collisionConfiguration;
		m_collisionConfiguration = 0;
	}


	virtual btSoftBodyWorldInfo* getSoftBodyWorldInfo()
	{
		return s_pBodyInfo;
	}
};

