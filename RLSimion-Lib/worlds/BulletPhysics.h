#pragma once
#include "../../3rd-party/bullet3-2.86/src/btBulletDynamicsCommon.h"

#include "BulletCreationInterface.h"

#include <stdio.h>
#include <vector>
#include <math.h>
class BulletBody;
class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;

class BulletPhysics : public BulletCreationInterface
{
	std::vector<btSoftBody*> m_pSoftObjects;

	std::vector<BulletBody*> m_bulletObjects;
public:

	BulletPhysics() : BulletCreationInterface() {}

	~BulletPhysics();

	virtual void initPhysics();
	virtual void initSoftPhysics();

	void simulate(double dt, int maxSubSteps);

	void connectWithRope(btRigidBody* body1, btRigidBody* body2);
	std::vector<btSoftBody*>* getSoftBodiesArray();

	void add(BulletBody* pBulletBody);
	void reset(CState* s);
	void updateState(CState* s);
	void updateBulletState(CState* s, const CAction* a, double dt);

	btAlignedObjectArray<btCollisionShape*> getCollisionShape();
};