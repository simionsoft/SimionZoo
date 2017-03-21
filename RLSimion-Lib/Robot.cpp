#include "stdafx.h"
#include "Robot.h"
#include "../3rd-party/bullet3-2.86/src/btBulletDynamicsCommon.h"


Robot::Robot(double mass, btDefaultMotionState* motionState, btCollisionShape* shape, btVector3& inertia)
{

	btRigidBody::btRigidBodyConstructionInfo Robot1rbInfo(mass, motionState, shape, inertia);
	m_pBody = new btRigidBody(Robot1rbInfo);
	m_force = btVector3(0.0, 0.0, 0.0);
}

void Robot::getAppliedForce(btVector3 &applied_force)
{
	applied_force = m_force;
}

void Robot::setAppliedForce(btVector3 &applied_force)
{
	m_force = applied_force;
}

btRigidBody* Robot::getBody(){
	return m_pBody;
}