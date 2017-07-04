#include "../stdafx.h"
#include "BulletPhysics.h"
#include "BulletBody.h"

// Initialization of physics
void BulletPhysics::initPhysics()
{
	createEmptyDynamicsWorld();
}

// Initialization of soft physics
void BulletPhysics::initSoftPhysics()
{
	createEmptySoftWorld();
}


// Needed getter in order to add shapes to the array of bullets collision shapes
btAlignedObjectArray<btCollisionShape*> BulletPhysics::getCollisionShape()
{
	return m_collisionShapes;
}

void BulletPhysics::add(BulletBody* pBulletObject)
{
	if (pBulletObject->bIsRigidBody())
	{
		if (pBulletObject->getShape())
			m_collisionShapes.push_back(pBulletObject->getShape());
		if (pBulletObject->getBody())
			m_dynamicsWorld->addRigidBody(pBulletObject->getBody());
	}

	m_bulletObjects.push_back(pBulletObject);
}

// Step the simulation 
void BulletPhysics::simulate(double dt, int maxSubSteps)
{
	stepSimulation((float)dt, maxSubSteps);

}


BulletPhysics::~BulletPhysics()
{
	//delete all bullet objects
	for (auto it = m_bulletObjects.begin(); it != m_bulletObjects.end(); ++it)
		delete (*it);

	exitPhysics();
}

void BulletPhysics::reset(CState* s)
{
	for (auto it = m_bulletObjects.begin(); it != m_bulletObjects.end(); ++it)
		(*it)->reset(s);
}

void BulletPhysics::updateState(CState* s)
{
	for (auto it = m_bulletObjects.begin(); it != m_bulletObjects.end(); ++it)
		(*it)->updateState(s);
}

void BulletPhysics::updateBulletState(CState* s, const CAction* a, double dt)
{
	for (auto it = m_bulletObjects.begin(); it != m_bulletObjects.end(); ++it)
		(*it)->updateBulletState(s,a,dt);
}
///////////////////////////////////////////
//CAN ONLY BE USED WITH SOFTDYNAMICSWORLD//
//////////////////////////////////////////

void BulletPhysics::connectWithRope(btRigidBody* body1, btRigidBody* body2)
{
	btSoftBody*	softBodyRope = btSoftBodyHelpers::CreateRope(*getSoftBodyWorldInfo()
		, body1->getWorldTransform().getOrigin(), body2->getWorldTransform().getOrigin(), 4, 0);
	softBodyRope->setTotalMass(0.1f);

	softBodyRope->appendAnchor(0, body1);
	softBodyRope->appendAnchor(softBodyRope->m_nodes.size() - 1, body2);

	softBodyRope->m_cfg.piterations = 5;
	softBodyRope->m_cfg.kDP = 0.005f;
	softBodyRope->m_cfg.kSHR = 1;
	softBodyRope->m_cfg.kCHR = 1;
	softBodyRope->m_cfg.kKHR = 1;
	getSoftDynamicsWorld()->addSoftBody(softBodyRope);
	m_pSoftObjects.push_back(softBodyRope);
}


std::vector<btSoftBody*>* BulletPhysics::getSoftBodiesArray()
{
	return &m_pSoftObjects;
}