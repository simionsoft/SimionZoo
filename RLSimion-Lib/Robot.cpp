#include "stdafx.h"
#include "Robot.h"
#include "../3rd-party/bullet3-2.86/src/btBulletDynamicsCommon.h"


Robot::Robot(double mass, btDefaultMotionState* motionState, btCollisionShape* shape, btVector3& inertia)
{

	btRigidBody::btRigidBodyConstructionInfo Robot1rbInfo(mass, motionState, shape, inertia);
	m_pBody = new btRigidBody(Robot1rbInfo);
	m_force = btVector3(0.0, 0.0, 0.0);
}

btRigidBody* Robot::getBody(){
	return m_pBody;
}