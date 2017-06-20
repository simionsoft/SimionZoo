#include "../stdafx.h"
#include "BulletBody.h"

BulletBody::BulletBody(double mass, const btVector3& origin, btCollisionShape* shape, bool moving_obj)
{
	m_shape = shape;
	
	m_transform.setIdentity();
	m_transform.setOrigin(origin);
	m_mass = btScalar(mass);

	m_localInertia = btVector3(0, 0, 0);
	bool isDynamic = (m_mass != 0.f);
	if (isDynamic)
	{
		m_shape->calculateLocalInertia(m_mass, m_localInertia);
	}
	btDefaultMotionState* motionState = new btDefaultMotionState(m_transform);
	btRigidBody::btRigidBodyConstructionInfo bulletBodyInfo(m_mass, motionState, m_shape, m_localInertia);
	m_pBody = new btRigidBody(bulletBodyInfo);
	if (moving_obj)
	{
		m_pBody->setActivationState(DISABLE_DEACTIVATION);
	}
	
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

void BulletBody::reset(CState* s, double originX, double originY, int idX, int idY)
{
	btTransform bodyTransform;
	
	btVector3 zeroVector(0, 0, 0);
	getBody()->clearForces();
	getBody()->setLinearVelocity(zeroVector);
	getBody()->setAngularVelocity(zeroVector);


	getBody()->getMotionState()->getWorldTransform(bodyTransform);
	bodyTransform.setOrigin(btVector3(originX, btScalar(0.0), originY));

	btQuaternion orientation = { 0.000000000, 0.000000000, 0.000000000, 1.00000000 };
	bodyTransform.setRotation(orientation);
	
	
	getBody()->setWorldTransform(bodyTransform);
	getBody()->getMotionState()->setWorldTransform(bodyTransform);

	
	s->set(idX, originX);
	s->set(idY, originY);

}

double BulletBody::updateRobotMovement(const CAction *a, CState *s, char *omega, char *vel, int theta, double dt)
{
	double rob_VelX, rob_VelY;

	double m_omega = a->get(omega);
	double linear_vel = a->get(vel);

	double m_theta = s->get(theta);
	m_theta += m_omega*dt;

	if (m_theta > SIMD_2_PI)
		m_theta -= SIMD_2_PI;
	if (m_theta < -SIMD_2_PI)
		m_theta += SIMD_2_PI;

	rob_VelX = cos(m_theta)*linear_vel;
	rob_VelY = sin(m_theta)*linear_vel;

	getBody()->setAngularVelocity(btVector3(0.0, m_omega, 0.0));
	getBody()->setLinearVelocity(btVector3(rob_VelX, 0.0, rob_VelY));

	return m_theta;
}

btTransform BulletBody::setAbsoluteVariables(CState* s, int idX, int idY)
{
	btTransform trans;
	getBody()->getMotionState()->getWorldTransform(trans);

	s->set(idX, float(trans.getOrigin().getX()));
	s->set(idY, float(trans.getOrigin().getZ()));

	return trans;
}


void BulletBody::setRelativeVariables(CState* s, int idX, int idY, bool isBox, double targetX, double targetY, double valX, double valY)
{
	btTransform bodyTrans;
	getBody()->getMotionState()->getWorldTransform(bodyTrans);
	
	if (!isBox)
	{
		s->set(idX, fabs(bodyTrans.getOrigin().getX() - valX));
		s->set(idY, fabs(bodyTrans.getOrigin().getZ() - valY));
	}
	else
	{
		s->set(idX, fabs(targetX- bodyTrans.getOrigin().getX()));
		s->set(idY, fabs(targetY- bodyTrans.getOrigin().getZ()));
	}
}

