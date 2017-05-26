#pragma once
#include "../3rd-party/bullet3-2.86/src/btBulletDynamicsCommon.h"

#include "BulletCreationInterface.h"

#include <stdio.h>
#include <vector>
#include <math.h>
#pragma comment(lib,"opengl32.lib")

class BulletPhysics : public BulletCreationInterface
{

	std::vector<btSoftBody*>*m_pSoftObjects;

public:

	BulletPhysics()
		:BulletCreationInterface()
	{	
		m_pSoftObjects = new std::vector<btSoftBody*>();
	}
	~BulletPhysics();

	virtual void initPhysics();
	virtual void initSoftPhysics();

	void simulate(double dt, int maxSubSteps);

	void connectWithRope(btRigidBody* body1, btRigidBody* body2);
	std::vector<btSoftBody*>* getSoftBodiesArray();
	btAlignedObjectArray<btCollisionShape*> getCollisionShape();
};