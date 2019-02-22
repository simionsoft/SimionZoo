/*
	SimionZoo: A framework for online model-free Reinforcement Learning on continuous
	control problems

	Copyright (c) 2016 SimionSoft. https://github.com/simionsoft

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#include "BulletBody.h"

BulletBody::BulletBody(double mass, const btVector3& origin, btCollisionShape* shape, int objType)
{
	m_shape = shape;
	
	btTransform trans;
	trans.setIdentity();
	trans.setOrigin(origin);
	m_originX = origin.x();
	m_originY = origin.z();
	m_originZ = origin.y();

	m_mass = btScalar(mass);

	//must be calculated before creating the rigid body
	resetInertia();
	
	btDefaultMotionState* motionState = new btDefaultMotionState(trans);
	btRigidBody::btRigidBodyConstructionInfo bulletBodyInfo(m_mass, motionState, m_shape, m_localInertia);
	m_pBody = new btRigidBody(bulletBodyInfo);
	m_pBody->setWorldTransform(trans);
	m_pBody->getMotionState()->setWorldTransform(trans);
	
	if (objType!=0)
		m_pBody->setCollisionFlags(objType);
	else m_pBody->setActivationState(DISABLE_DEACTIVATION);
}

void BulletBody::resetInertia()
{
	m_localInertia = btVector3(0, 0, 0);

	if (m_mass != 0.f) //is a dynamic object?
		m_shape->calculateLocalInertia(m_mass, m_localInertia);
}

btRigidBody* BulletBody::getBody()
{
	return m_pBody;
}

btCollisionShape* BulletBody::getShape()
{
	return m_shape;
}

void BulletBody::setAbsoluteStateVarIds(const char* xId, const char* yId, const char* thetaId)
{
	m_xId = xId; m_yId = yId; m_thetaId = thetaId;
	m_bAbsVariablesSet = true;
	m_bAngleSet = true;
}
void BulletBody::setAbsoluteStateVarIds(const char* xId, const char* yId)
{
	m_xId = xId; m_yId = yId;
	m_bAbsVariablesSet = true;
	m_bAngleSet = false;
}

void BulletBody::setRelativeStateVarIds(const char* relXId, const char* relYId, const char* refXId, const char* refYId)
{
	m_relXId = relXId; m_relYId = relYId; m_refXId = refXId; m_refYId = refYId;
	m_bRelVariablesSet = true;
}
void BulletBody::setOrigin(double x, double y, double theta)
{
	m_originX = x; m_originY = y; m_originTheta = theta;
}


void BulletBody::reset(State* s)
{
	btTransform bodyTransform;
	btQuaternion orientation;

	btVector3 zeroVector(0, 0, 0);
	m_pBody->clearForces();
	m_pBody->setLinearVelocity(zeroVector);
	m_pBody->setAngularVelocity(zeroVector);

	bodyTransform= m_pBody->getWorldTransform();
	bodyTransform.setOrigin(btVector3(m_originX, m_originZ, m_originY));
	orientation.setEuler(m_originTheta, 0.0, 0.0);
	bodyTransform.setRotation(orientation);
	
	m_pBody->setWorldTransform(bodyTransform);
	m_pBody->getMotionState()->setWorldTransform(bodyTransform);

	resetInertia();

	updateState(s);
}

void BulletBody::updateState(State* s)
{
	btTransform trans;
	if (areAbsVariablesSet())
	{
		m_pBody->getMotionState()->getWorldTransform(trans);

		if (m_xId >= 0) s->set(m_xId, float(trans.getOrigin().getX()));
		if (m_yId >= 0) s->set(m_yId, float(trans.getOrigin().getZ()));
		updateYawState(s);
	}
	if (areRelVariablesSet())
	{
		s->set(m_relXId, s->get(m_refXId) - s->get(m_xId));
		s->set(m_relYId, s->get(m_refYId) - s->get(m_yId));
	}
}

void BulletBody::updateYawState(State* s)
{
	if (isAngleSet())
	{
		btTransform transform;
		m_pBody->getMotionState()->getWorldTransform(transform);
		btScalar yaw, pitch, roll;
		transform.getBasis().getEulerYPR(yaw, pitch, roll);

		s->set(m_thetaId, (double)yaw);
	}
}
