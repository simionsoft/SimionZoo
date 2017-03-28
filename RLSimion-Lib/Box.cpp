#include "stdafx.h"
#include "Box.h"
#include "../3rd-party/bullet3-2.86/src/btBulletDynamicsCommon.h"


Box::Box(double mass, btDefaultMotionState* motionState, btCollisionShape* shape, btVector3& inertia)
{
	btRigidBody::btRigidBodyConstructionInfo Robot1rbInfo(mass, motionState, shape, inertia);
	m_pBody = new btRigidBody(Robot1rbInfo);
}


btRigidBody* Box::getBody() {
	return m_pBody;
}