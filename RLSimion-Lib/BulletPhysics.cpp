#include "stdafx.h"
#include "BulletPhysics.h"

#pragma comment(lib,"opengl32.lib")

// Inicialization of physics
void BulletPhysics::initPhysics()
{
	createEmptyDynamicsWorld();
}

// Inicialization of soft physics
void BulletPhysics::initSoftPhysics(btSoftBodyWorldInfo* m_sBodyWorldInfo)
{
	createEmptySoftWorld(m_sBodyWorldInfo);
}

// Step the simulation 
void BulletPhysics::simulate(double dt)
{
	stepSimulation(dt);
}

// Needed getter in order to add shapes to the array of bullets collision shapes
btAlignedObjectArray<btCollisionShape*> BulletPhysics::getCollisionShape()
{
	return m_collisionShapes;
}

BulletPhysics::~BulletPhysics()
{
	exitPhysics();
}


///////////////////////////////////////////
//ONLY CAN BE USED WITH SOFTDYNAMICSWORLD//
//////////////////////////////////////////

void BulletPhysics::connectWithRope(btRigidBody* body1, btRigidBody* body2, btSoftBodyWorldInfo* btInfo)
{
	//btSoftBodyWorldInfo btInf = btInfo;
	btSoftBody*	softBodyRope0 = btSoftBodyHelpers::CreateRope(*btInfo, body1->getWorldTransform().getOrigin(), body2->getWorldTransform().getOrigin(), 4, 0);
	softBodyRope0->setTotalMass(0.1f);

	softBodyRope0->appendAnchor(0, body1);
	softBodyRope0->appendAnchor(softBodyRope0->m_nodes.size() - 1, body2);

	softBodyRope0->m_cfg.piterations = 5;
	softBodyRope0->m_cfg.kDP = 0.005f;
	softBodyRope0->m_cfg.kSHR = 1;
	softBodyRope0->m_cfg.kCHR = 1;
	softBodyRope0->m_cfg.kKHR = 1;
	getSoftDynamicsWorld()->addSoftBody(softBodyRope0);
}