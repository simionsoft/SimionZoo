/*
Bullet Continuous Collision Detection and Physics Library
Ragdoll Demo
Copyright (c) 2007 Starbreeze Studios

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it freely,
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

Written by: Marten Svanfeldt
*/

#include "drone-6-dof.h"
#include "BulletPhysics.h"


//#define RIGID 1

Drone6DOF::Drone6DOF (BulletPhysics* physics, const btVector3& positionOffset) :BulletBody(positionOffset)
{
	m_physics = physics;
		
	// Setup the geometry

	m_shapes[BASE] = new btBoxShape(btVector3(btScalar(m_baseHalfWidth), btScalar(m_baseHalfHeight),btScalar(m_baseHalfWidth)));
	m_shapes[DRONE_1] = new btBoxShape(btVector3(btScalar(m_droneWidth), btScalar(m_droneHeight), btScalar(m_droneWidth)));
	m_shapes[DRONE_2] = new btBoxShape(btVector3(btScalar(m_droneWidth), btScalar(m_droneHeight), btScalar(m_droneWidth)));
	m_shapes[DRONE_3] = new btBoxShape(btVector3(btScalar(m_droneWidth), btScalar(m_droneHeight), btScalar(m_droneWidth)));
	m_shapes[DRONE_4] = new btBoxShape(btVector3(btScalar(m_droneWidth), btScalar(m_droneHeight), btScalar(m_droneWidth)));

	m_masses[BASE] = btScalar(m_baseMass);
	m_masses[DRONE_1] = btScalar(m_droneMass);
	m_masses[DRONE_2] = btScalar(m_droneMass);
	m_masses[DRONE_3] = btScalar(m_droneMass);
	m_masses[DRONE_4] = btScalar(m_droneMass);

	m_origins[BASE] = btVector3(positionOffset);
	m_origins[DRONE_1] = btVector3(positionOffset + btVector3(-m_jointPositionOffset, m_droneRelPosY, -m_jointPositionOffset));
	m_origins[DRONE_2] = btVector3(positionOffset + btVector3(-m_jointPositionOffset, m_droneRelPosY, m_jointPositionOffset));
	m_origins[DRONE_3] = btVector3(positionOffset + btVector3(m_jointPositionOffset, m_droneRelPosY, m_jointPositionOffset));
	m_origins[DRONE_4] = btVector3(positionOffset + btVector3(m_jointPositionOffset, m_droneRelPosY, -m_jointPositionOffset));

	for (int i = 0; i < FORCE_COUNT; i++)
		m_forces[i] = 0.0;
	
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(m_origins[BASE]);
	m_bodies[BASE] = localCreateRigidBody(btScalar(10.), transform, m_shapes[BASE]);

	transform.setIdentity();
	transform.setOrigin(m_origins[DRONE_1]);
	m_bodies[DRONE_1] = localCreateRigidBody(btScalar(0.5), transform, m_shapes[DRONE_1]);
	
	transform.setIdentity();
	transform.setOrigin(m_origins[DRONE_2]);
	m_bodies[DRONE_2] = localCreateRigidBody(btScalar(0.5), transform, m_shapes[DRONE_2]);

	transform.setIdentity();
	transform.setOrigin(m_origins[DRONE_3]);
	m_bodies[DRONE_3] = localCreateRigidBody(btScalar(0.5), transform, m_shapes[DRONE_3]);

	transform.setIdentity();
	transform.setOrigin(m_origins[DRONE_4]);
	m_bodies[DRONE_4] = localCreateRigidBody(btScalar(0.5), transform, m_shapes[DRONE_4]);

	
///////////////////////////// CONSTRAINTS /////////////////////////////////////////////7777
	
	btGeneric6DofSpringConstraint* joint;
	btTransform localA, localB;
	bool useLinearReferenceFrameA = true;
	const double angularLowerLimitXZ = -SIMD_HALF_PI * 0.3;
	const double angularUpperLimitXZ = SIMD_HALF_PI * 0.3;
	const double angularLowerLimitY = -SIMD_HALF_PI * 0.3;
	const double angularUpperLimitY = SIMD_HALF_PI * 0.3;
	const double linearUpperLimit = 0 + 0.1;
	const double linearLowerLimit = 0;

	
	{
		localA.setIdentity(); localB.setIdentity();
		localA.setOrigin(btVector3(btScalar(-m_jointPositionOffset), btScalar(m_droneRelPosY), btScalar(-m_jointPositionOffset)));
		localB.setOrigin(btVector3(btScalar(0.), btScalar(-m_droneHeight), btScalar(0.)));
		joint = new btGeneric6DofSpringConstraint(*m_bodies[BASE], *m_bodies[DRONE_1], localA, localB, useLinearReferenceFrameA);
		joint->setAngularLowerLimit(btVector3(angularLowerLimitXZ, angularLowerLimitY, angularLowerLimitXZ));
		joint->setAngularUpperLimit(btVector3(angularUpperLimitXZ, angularUpperLimitY, angularUpperLimitXZ));
		joint->setLinearLowerLimit(btVector3(linearLowerLimit, linearLowerLimit, linearLowerLimit));
		joint->setLinearUpperLimit(btVector3(linearUpperLimit, linearUpperLimit, linearUpperLimit));
		m_joints[JOINT_DRONE_1] = joint;
		physics->getDynamicsWorld()->addConstraint(m_joints[JOINT_DRONE_1], true);
				
		localA.setIdentity(); localB.setIdentity();
		localA.setOrigin(btVector3(btScalar(-m_jointPositionOffset), btScalar(m_droneRelPosY), btScalar(m_jointPositionOffset)));
		localB.setOrigin(btVector3(btScalar(0.), btScalar(-m_droneHeight), btScalar(0.)));
		joint = new btGeneric6DofSpringConstraint(*m_bodies[BASE], *m_bodies[DRONE_2], localA, localB, useLinearReferenceFrameA);
		joint->setAngularLowerLimit(btVector3(angularLowerLimitXZ, angularLowerLimitY, angularLowerLimitXZ));
		joint->setAngularUpperLimit(btVector3(angularUpperLimitXZ, angularUpperLimitY, angularUpperLimitXZ));
		joint->setLinearLowerLimit(btVector3(linearLowerLimit, linearLowerLimit, linearLowerLimit));
		joint->setLinearUpperLimit(btVector3(linearUpperLimit, linearUpperLimit, linearUpperLimit));
		m_joints[JOINT_DRONE_2] = joint;
		physics->getDynamicsWorld()->addConstraint(m_joints[JOINT_DRONE_2], true);

		localA.setIdentity(); localB.setIdentity();
		localA.setOrigin(btVector3(btScalar(m_jointPositionOffset), btScalar(m_droneRelPosY), btScalar(m_jointPositionOffset)));
		localB.setOrigin(btVector3(btScalar(0.), btScalar(-m_droneHeight), btScalar(0.)));
		joint = new btGeneric6DofSpringConstraint(*m_bodies[BASE], *m_bodies[DRONE_3], localA, localB, useLinearReferenceFrameA);
		joint->setAngularLowerLimit(btVector3(angularLowerLimitXZ, angularLowerLimitY, angularLowerLimitXZ));
		joint->setAngularUpperLimit(btVector3(angularUpperLimitXZ, angularUpperLimitY, angularUpperLimitXZ));
		joint->setLinearLowerLimit(btVector3(linearLowerLimit, linearLowerLimit, linearLowerLimit));
		joint->setLinearUpperLimit(btVector3(linearUpperLimit, linearUpperLimit, linearUpperLimit));
		m_joints[JOINT_DRONE_3] = joint;
		physics->getDynamicsWorld()->addConstraint(m_joints[JOINT_DRONE_3], true);

		localA.setIdentity(); localB.setIdentity();
		localA.setOrigin(btVector3(btScalar(m_jointPositionOffset), btScalar(m_droneRelPosY), btScalar(-m_jointPositionOffset)));
		localB.setOrigin(btVector3(btScalar(0.), btScalar(-m_droneHeight), btScalar(0.)));
		joint = new btGeneric6DofSpringConstraint(*m_bodies[BASE], *m_bodies[DRONE_4], localA, localB, useLinearReferenceFrameA);
		joint->setAngularLowerLimit(btVector3(angularLowerLimitXZ, angularLowerLimitY, angularLowerLimitXZ));
		joint->setAngularUpperLimit(btVector3(angularUpperLimitXZ, angularUpperLimitY, angularUpperLimitXZ));
		joint->setLinearLowerLimit(btVector3(linearLowerLimit, linearLowerLimit, linearLowerLimit));
		joint->setLinearUpperLimit(btVector3(linearUpperLimit, linearUpperLimit, linearUpperLimit));
		m_joints[JOINT_DRONE_4] = joint;
		physics->getDynamicsWorld()->addConstraint(m_joints[JOINT_DRONE_4], true);
	}
}

Drone6DOF::~Drone6DOF()
{
	int i;

	// Remove all constraints
	for (i = 0; i < JOINT_COUNT; ++i)
	{
		m_physics->getDynamicsWorld()->removeConstraint(m_joints[i]);
		delete m_joints[i]; m_joints[i] = 0;
	}
	
	// Remove all bodies
	for (i = 0; i < DRONE_ELEMENT_COUNT; ++i)
	{
		m_physics->getDynamicsWorld()->removeRigidBody(m_bodies[i]);
		delete m_bodies[i]->getMotionState();
		delete m_bodies[i]; m_bodies[i] = 0;
	}
}


btRigidBody* Drone6DOF::localCreateRigidBody (btScalar mass, const btTransform& startTransform, btCollisionShape* shape)
{
	btVector3 localInertia(0,0,0);
	bool isDynamic = (mass != 0.f);
	if (isDynamic)
		shape->calculateLocalInertia(mass, localInertia);

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, shape, localInertia);
	rbInfo.m_additionalDamping = true;
	btRigidBody* body = new btRigidBody(rbInfo);
	body->setActivationState(DISABLE_DEACTIVATION);
	m_physics->add(shape, body);
	return body;
}


void Drone6DOF::setAbsoluteStateVarIds(const char * xId, const char * yId, const char * zId, const char * rotXId, const char * rotYId, const char * rotZId, const char * angularVXId, const char * angularVYId, const char * angularVZId, const char * linearVXId, const char * linearVYId, const char * linearVZId,
	const char * x1Id, const char * y1Id, const char * z1Id, const char * rotX1Id, const char * rotY1Id, const char * rotZ1Id, const char * angularVX1Id, const char * angularVY1Id, const char * angularVZ1Id, const char * linearVX1Id, const char * linearVY1Id, const char * linearVZ1Id,
	const char * x2Id, const char * y2Id, const char * z2Id, const char * rotX2Id, const char * rotY2Id, const char * rotZ2Id, const char * angularVX2Id, const char * angularVY2Id, const char * angularVZ2Id, const char * linearVX2Id, const char * linearVY2Id, const char * linearVZ2Id,
	const char * x3Id, const char * y3Id, const char * z3Id, const char * rotX3Id, const char * rotY3Id, const char * rotZ3Id, const char * angularVX3Id, const char * angularVY3Id, const char * angularVZ3Id, const char * linearVX3Id, const char * linearVY3Id, const char * linearVZ3Id,
	const char * x4Id, const char * y4Id, const char * z4Id, const char * rotX4Id, const char * rotY4Id, const char * rotZ4Id, const char * angularVX4Id, const char * angularVY4Id, const char * angularVZ4Id, const char * linearVX4Id, const char * linearVY4Id, const char * linearVZ4Id)
{
	m_xId = xId; 	m_yId = yId; 	m_zId = zId;
	m_rotXId = rotXId; 	m_rotYId = rotYId; 	m_rotZId = rotZId;
	m_angularVXId = angularVXId; m_angularVYId = angularVYId; m_angularVZId = angularVZId;
	m_linearVXId = linearVXId; m_linearVYId = linearVYId; m_linearVZId = linearVZId;

	m_x1Id = x1Id; 	m_y1Id = y1Id; 	m_z1Id = z1Id;
	m_rotX1Id = rotX1Id; 	m_rotY1Id = rotY1Id; 	m_rotZ1Id = rotZ1Id;
	m_angularVX1Id = angularVX1Id; m_angularVY1Id = angularVY1Id; m_angularVZ1Id = angularVZ1Id;
	m_linearVX1Id = linearVX1Id; m_linearVY1Id = linearVY1Id; m_linearVZ1Id = linearVZ1Id;


	m_x2Id = x2Id; 	m_y2Id = y2Id; 	m_z2Id = z2Id;
	m_rotX2Id = rotX2Id; 	m_rotY2Id = rotY2Id; 	m_rotZ2Id = rotZ2Id;
	m_angularVX2Id = angularVX2Id; m_angularVY2Id = angularVY2Id; m_angularVZ2Id = angularVZ2Id;
	m_linearVX2Id = linearVX2Id; m_linearVY2Id = linearVY2Id; m_linearVZ2Id = linearVZ2Id;

	m_x3Id = x3Id; 	m_y3Id = y3Id; 	m_z3Id = z3Id;
	m_rotX3Id = rotX3Id; 	m_rotY3Id = rotY3Id; 	m_rotZ3Id = rotZ3Id;
	m_angularVX3Id = angularVX3Id; m_angularVY3Id = angularVY3Id; m_angularVZ3Id = angularVZ3Id;
	m_linearVX3Id = linearVX3Id; m_linearVY3Id = linearVY3Id; m_linearVZ3Id = linearVZ3Id;

	m_x4Id = x4Id; 	m_y4Id = y4Id; 	m_z4Id = z4Id;
	m_rotX4Id = rotX4Id; 	m_rotY4Id = rotY4Id; 	m_rotZ4Id = rotZ4Id;	
	m_angularVX4Id = angularVX4Id; m_angularVY4Id = angularVY4Id; m_angularVZ4Id = angularVZ4Id;
	m_linearVX4Id = linearVX4Id; m_linearVY4Id = linearVY4Id; m_linearVZ4Id = linearVZ4Id;

}

void Drone6DOF::setErrorStateVarId(const char * id)
{
	m_error = id;
}

void Drone6DOF::updateBulletState(State * s, const Action * a, double dt)
{
	m_forces[F1_1] = a->get(m_f1_1Id);
	m_forces[F1_2] = a->get(m_f1_2Id);
	m_forces[F1_3] = a->get(m_f1_3Id);
	m_forces[F1_4] = a->get(m_f1_4Id);
		
	m_forces[F2_1] = a->get(m_f2_1Id);
	m_forces[F2_2] = a->get(m_f2_2Id);
	m_forces[F2_3] = a->get(m_f2_3Id);
	m_forces[F2_4] = a->get(m_f2_4Id);

	m_forces[F3_1] = a->get(m_f3_1Id);
	m_forces[F3_2] = a->get(m_f3_2Id);
	m_forces[F3_3] = a->get(m_f3_3Id);
	m_forces[F3_4] = a->get(m_f3_4Id);

	m_forces[F4_1] = a->get(m_f4_1Id);
	m_forces[F4_2] = a->get(m_f4_2Id);
	m_forces[F4_3] = a->get(m_f4_3Id);
	m_forces[F4_4] = a->get(m_f4_4Id);
	
	int j = 0;
	for (size_t i = 1; i < DRONE_ELEMENT_COUNT; i++)
	{
		btVector3 relativeForce = btVector3(0., 1.0, 0.);
		btMatrix3x3& boxRot = m_bodies[i]->getWorldTransform().getBasis();
		
		btVector3 forceVector = boxRot * (relativeForce * m_forces[j]);
		m_bodies[i]->applyForce(forceVector, btVector3(-0.3, -m_droneHeight, -0.3));
		j ++;

		forceVector = boxRot * (relativeForce * m_forces[j]);
		m_bodies[i]->applyForce(forceVector, btVector3(-0.3, -m_droneHeight, 0.3));
		j++;

		forceVector = boxRot * (relativeForce * m_forces[j]);
		m_bodies[i]->applyForce(forceVector, btVector3(0.3, -m_droneHeight, 0.3));
		j++;

		forceVector = boxRot * (relativeForce * m_forces[j]);
		m_bodies[i]->applyForce(forceVector, btVector3(0.3, -m_droneHeight, -0.3));
		j++;
	}


}
void Drone6DOF::reset(State * s)
{
	double height = s->get("base-y");
	s->set("d-error-z", 0);
	btTransform bodyTransform;
	btQuaternion orientation;
	btVector3 zeroVector(0, 0, 0);
	for (size_t i = 0; i < DRONE_ELEMENT_COUNT; i++)
	{
		m_bodies[i]->clearForces();
		m_bodies[i]->setLinearVelocity(zeroVector);
		m_bodies[i]->setAngularVelocity(zeroVector);

		bodyTransform = m_bodies[i]->getWorldTransform();
		bodyTransform.setOrigin(m_origins[i] + btVector3(0.0, height, 0.0));
		orientation.setEuler(0.0, 0.0, 0.0);
		bodyTransform.setRotation(orientation);

		m_bodies[i]->setWorldTransform(bodyTransform);
		m_bodies[i]->getMotionState()->setWorldTransform(bodyTransform);

		btVector3 localInertia(0, 0, 0);
		m_bodies[i]->getCollisionShape()->calculateLocalInertia(m_masses[i], localInertia);
	}
	
	updateState(s);
}
void Drone6DOF::updateState(State * s)
{
	btTransform transform = m_bodies[BASE]->getWorldTransform();
	
	s->set(m_xId, transform.getOrigin().x());
	s->set(m_yId, transform.getOrigin().y());
	s->set(m_zId, transform.getOrigin().z());

	s->set(m_error, height- transform.getOrigin().y() );
	s->set("errorX", m_origins[0].x() - transform.getOrigin().x());
	s->set("errorY", m_origins[0].z() - transform.getOrigin().z());


	btMatrix3x3& boxRot = transform.getBasis();


	btScalar x,y,z; 
	boxRot.getEulerZYX(z, y, x);

	s->set(m_rotXId, x);
	s->set(m_rotYId, y);
	s->set(m_rotZId, z);

	btVector3 velocity = m_bodies[BASE]->getAngularVelocity();
	s->set(m_angularVXId, velocity.x());
	s->set(m_angularVYId, velocity.y());
	s->set(m_angularVZId, velocity.z());

	velocity = m_bodies[BASE]->getLinearVelocity();
	s->set(m_linearVXId, velocity.x());
	s->set(m_linearVYId, velocity.y());
	s->set(m_linearVZId, velocity.z());

	for (size_t i = 1; i < DRONE_ELEMENT_COUNT; i++)
	{
		transform = m_bodies[i]->getWorldTransform();
		
		s->set((string("drone") + to_string(i) + string("-x")).c_str(), transform.getOrigin().x());
		s->set((string("drone") + to_string(i) + string("-y")).c_str(), transform.getOrigin().y());
		s->set((string("drone") + to_string(i) + string("-z")).c_str(), transform.getOrigin().z());

		boxRot = transform.getBasis();
		boxRot.getEulerZYX(z, y, x);

		s->set((string("drone") + to_string(i) + string("-rot-x")).c_str(), x);
		s->set((string("drone") + to_string(i) + string("-rot-y")).c_str(), y);
		s->set((string("drone") + to_string(i) + string("-rot-z")).c_str(), z);
		
		velocity = m_bodies[i]->getAngularVelocity();
		s->set((string("drone") + to_string(i) + string("-angular-x")).c_str(), velocity.x());
		s->set((string("drone") + to_string(i) + string("-angular-y")).c_str(), velocity.y());
		s->set((string("drone") + to_string(i) + string("-angular-z")).c_str(), velocity.z());

		velocity = m_bodies[i]->getLinearVelocity();
		s->set((string("drone") + to_string(i) + string("-linear-x")).c_str(), velocity.x());
		s->set((string("drone") + to_string(i) + string("-linear-y")).c_str(), velocity.y());
		s->set((string("drone") + to_string(i) + string("-linear-z")).c_str(), velocity.z());
		
	}
	
}


