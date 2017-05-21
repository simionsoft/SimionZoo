#pragma once
#include "../3rd-party/bullet3-2.86/src/btBulletDynamicsCommon.h"

#include "BulletCreationInterface.h"

#include <stdio.h>
#include <math.h>
#pragma comment(lib,"opengl32.lib")

class BulletPhysics : public BulletCreationInterface
{
	
public:

	BulletPhysics()
		:BulletCreationInterface()
	{	
	}
	~BulletPhysics();

	virtual void initPhysics();
	virtual void initSoftPhysics();

	void simulate(double dt);

	void connectWithRope(btRigidBody* body1, btRigidBody* body2);

	btAlignedObjectArray<btCollisionShape*> getCollisionShape();
};