#include "stdafx.h"
#include "BulletBody.h"

BulletBody::BulletBody(double mass,double xPos, double yPos, btCollisionShape* shape)
{
	m_shape = shape;
	
	m_transform.setIdentity();
	m_transform.setOrigin(btVector3(xPos, 0.0, yPos));
	m_mass = mass;

	m_localInertia = btVector3(0, 0, 0);
	bool isDynamic = (m_mass != 0.f);
	if (isDynamic)
	{
		m_shape->calculateLocalInertia(m_mass, m_localInertia);
	}
	btDefaultMotionState* motionState = new btDefaultMotionState(m_transform);
	btRigidBody::btRigidBodyConstructionInfo bulletBodyInfo(m_mass, motionState, m_shape, m_localInertia);
	m_pBody = new btRigidBody(bulletBodyInfo);

}

btRigidBody* BulletBody::getBody()
{
	return m_pBody;
}

btCollisionShape* BulletBody::getShape()
{
	return m_shape;
}

btTransform BulletBody::getTransform()
{
	return m_transform;
}