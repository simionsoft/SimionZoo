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
	//CONSTANTS
	static const double MASS_ROBOT;
	static const double MASS_GROUND;
	static const double MASS_TARGET;
	static const double MASS_BOX;

	static const double TargetX;
	static const double TargetY;
	static const double TargetZ;

	static const double ground_x;
	static const double ground_y;
	static const double ground_z;

	static const double r1origin_x;
	static const double r1origin_y;
	static const double r1origin_z;

	static const double r2origin_x;
	static const double r2origin_y;
	static const double r2origin_z;

	static const double boxOrigin_x;
	static const double boxOrigin_y;
	static const double boxOrigin_z;

	static const double theta_o1;
	static const double theta_o2;


	BulletPhysics() : BulletCreationInterface() {}
	~BulletPhysics();

	virtual void initPhysics();
	virtual void initSoftPhysics();
	void initPlayground();

	void simulate(double dt, int maxSubSteps);

	void connectWithRope(btRigidBody* body1, btRigidBody* body2);
	std::vector<btSoftBody*>* getSoftBodiesArray();

	void add(BulletBody* pBulletBody);
	void reset(CState* s);
	void updateState(CState* s);
	void updateBulletState(CState* s, const CAction* a, double dt);

	btAlignedObjectArray<btCollisionShape*> getCollisionShape();
};