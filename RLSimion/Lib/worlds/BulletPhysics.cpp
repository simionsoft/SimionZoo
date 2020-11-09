/*
	SimionZoo: A framework for online model-free Reinforcement Learning on continuous
	control problems

	Copyright (c) 2016 SimionSoft. https://github.com/simionsoft

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#include "BulletPhysics.h"
#include "BulletBody.h"
#include "Box.h"

//static public constants
const double BulletPhysics::MASS_ROBOT = 0.5f;
const double BulletPhysics::MASS_TARGET = 0.1f;
const double BulletPhysics::MASS_BOX = 4.0;

const double BulletPhysics::TargetX = 12.0;
const double BulletPhysics::TargetY = -2.0;
const double BulletPhysics::TargetZ = 0.5;

const double BulletPhysics::ground_x = 0.0;
const double BulletPhysics::ground_y = -50.0;
const double BulletPhysics::ground_z = 0.0;

const double BulletPhysics::r1origin_x = 0.0;
const double BulletPhysics::r1origin_y = 0.0;
const double BulletPhysics::r1origin_z = 0.5;

const double BulletPhysics::r2origin_x = 3.0;
const double BulletPhysics::r2origin_y = 4.0;
const double BulletPhysics::r2origin_z = 0.5;

const double BulletPhysics::boxOrigin_x = 3.0;
const double BulletPhysics::boxOrigin_y = 2.0;
const double BulletPhysics::boxOrigin_z = 0.5;

const double BulletPhysics::theta_o1 = 0.0;
const double BulletPhysics::theta_o2 = 0.0;

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
		//soft bodies (rope) don't need to be added, as they will be added in their constructor
		if (pBulletObject->getShape())
			m_collisionShapes.push_back(pBulletObject->getShape());
		if (pBulletObject->getBody())
			m_dynamicsWorld->addRigidBody(pBulletObject->getBody());
	}

	m_bulletObjects.push_back(pBulletObject);
}

void BulletPhysics::add(btCollisionShape* shape, btRigidBody* rig)
{
	m_collisionShapes.push_back(shape);
	m_dynamicsWorld->addRigidBody(rig);
}

void BulletPhysics::addBody(BulletBody * pBulletBody)
{
	m_bulletObjects.push_back(pBulletBody);
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
void BulletPhysics::initPlayground()
{
	///Creating static object, ground
	{
		StaticObject* pGround = new StaticObject(btVector3(ground_x, ground_y, ground_z)
			, new btBoxShape(btVector3(btScalar(50.), btScalar(50.), btScalar(50.))));
		add(pGround);
	}
	///Creating static object, walls
	{
		StaticObject* pWall = new StaticObject(btVector3(12.0, 1.0, 0.0)
			, new btBoxShape(btVector3(1.0, 2.0, 13.)));
		add(pWall);
		pWall = new StaticObject(btVector3(-12.0, 1.0, 0.0)
			, new btBoxShape(btVector3(1.0, 2.0, 13.)));
		add(pWall);
		pWall = new StaticObject(btVector3(1.0, 1.0, 12.0)
			, new btBoxShape(btVector3(12.0, 2.0, 1.0)));
		add(pWall);
		pWall = new StaticObject(btVector3(1.0, 1.0, -12.0)
			, new btBoxShape(btVector3(12.0, 2.0, 1.0)));
		add(pWall);
	}
}

void BulletPhysics::reset(State* s)
{
	for (auto it = m_bulletObjects.begin(); it != m_bulletObjects.end(); ++it)
		(*it)->reset(s);
}

void BulletPhysics::updateState(State* s)
{
	for (auto it = m_bulletObjects.begin(); it != m_bulletObjects.end(); ++it)
		(*it)->updateState(s);
}

void BulletPhysics::updateBulletState(State* s, const Action* a, double dt)
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