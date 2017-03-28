#pragma once
#include "../3rd-party/bullet3-2.86/src/btBulletDynamicsCommon.h"


class Robot {
public:
	Robot(double mass, btDefaultMotionState* motionState, btCollisionShape* shape, btVector3& inertia);
	virtual ~Robot() { }

	btRigidBody* Robot::getBody();

protected:
	btVector3 m_force;
	btRigidBody* m_pBody;
};