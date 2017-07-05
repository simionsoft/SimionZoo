#include "../stdafx.h"
#include "BulletBody.h"

BulletBody::BulletBody(double mass, const btVector3& origin, btCollisionShape* shape, int objType)
{
	m_shape = shape;
	
	btTransform trans;
	trans.setIdentity();
	trans.setOrigin(origin);
	m_originX = origin.x();
	m_originY = origin.y();

	m_mass = btScalar(mass);

	btVector3 localInertia = btVector3(0, 0, 0);
	bool isDynamic = (m_mass != 0.f);
	
	if (isDynamic)
		m_shape->calculateLocalInertia(m_mass, localInertia);
	
	btDefaultMotionState* motionState = new btDefaultMotionState(trans);
	btRigidBody::btRigidBodyConstructionInfo bulletBodyInfo(m_mass, motionState, m_shape, localInertia);
	m_pBody = new btRigidBody(bulletBodyInfo);
	if ((objType | btCollisionObject::CF_STATIC_OBJECT) == 0)
	{
		m_pBody->setActivationState(DISABLE_DEACTIVATION);
	}
	if (objType!=0)
		m_pBody->setCollisionFlags(objType);
	m_objType = objType;
}


btRigidBody* BulletBody::getBody()
{
	return m_pBody;
}

btCollisionShape* BulletBody::getShape()
{
	return m_shape;
}

void BulletBody::setAbsoluteStateVarIds(int xId, int yId, int thetaId)
{
	m_xId = xId; m_yId = yId; m_thetaId = thetaId;
	m_bSetAbsStateVars = true;
}
void BulletBody::setRelativeStateVarIds(int relXId, int relYId, int refXId, int refYId)
{
	m_relXId = relXId; m_relYId = relYId; m_refXId = refXId; m_refYId = refYId;
	m_bSetRelStateVars = true;
}
void BulletBody::setOrigin(double x, double y, double theta)
{
	m_originX = x; m_originY = y; m_originTheta = theta;
}


void BulletBody::reset(CState* s)
{
	btTransform bodyTransform;
	
	btVector3 zeroVector(0, 0, 0);
	//if (!m_objType)
	{
		m_pBody->clearForces();
		m_pBody->setLinearVelocity(zeroVector);
		m_pBody->setAngularVelocity(zeroVector);
	}

	m_pBody->getMotionState()->getWorldTransform(bodyTransform);
	bodyTransform.setOrigin(btVector3(m_originX, 0.0, m_originY));

	btQuaternion orientation = { 0.000000000, 0.000000000, 0.000000000, 1.00000000 };
	orientation.setEuler(m_originTheta, 0.0, 0.0);
	bodyTransform.setRotation(orientation);
	
	m_pBody->setWorldTransform(bodyTransform);
	m_pBody->getMotionState()->setWorldTransform(bodyTransform);

	updateState(s);
}

void BulletBody::updateState(CState* s)
{
	btTransform trans;
	if (bSetAbsStateVars())
	{
		m_pBody->getMotionState()->getWorldTransform(trans);

		if (m_xId >= 0) s->set(m_xId, float(trans.getOrigin().getX()));
		if (m_yId >= 0) s->set(m_yId, float(trans.getOrigin().getZ()));
		updateYawState(s);
	}
	if (bSetRelStateVars())
	{
		s->set(m_relXId, s->get(m_refXId) - s->get(m_xId));
		s->set(m_relYId, s->get(m_refYId) - s->get(m_yId));
	}
}

void BulletBody::updateYawState(CState* s)
{
	if (m_thetaId >= 0)
	{
		btTransform transform;
		m_pBody->getMotionState()->getWorldTransform(transform);
		btScalar yaw, pitch, roll;
		transform.getBasis().getEulerYPR(yaw, pitch, roll);

		s->set(m_thetaId, (double)yaw);
	}
}
