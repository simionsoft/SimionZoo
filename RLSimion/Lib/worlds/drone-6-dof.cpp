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
	fisicas = physics;
		
	// Setup the geometry
	m_shapes[BODYPART_BASE] = new btBoxShape(btVector3(btScalar(3.0),btScalar( 0.1),btScalar(3.0)));
	m_shapes[BODYPART_LEFT_UPP] = new btBoxShape(btVector3(btScalar(0.5 ), btScalar(0.02 ), btScalar( 0.5)));
	m_shapes[BODYPART_RIGHT_UPP] = new btBoxShape(btVector3(btScalar(0.5 ), btScalar(0.02 ), btScalar( 0.5)));
	m_shapes[BODYPART_LEFT_LOW] = new btBoxShape(btVector3(btScalar(0.5 ), btScalar(0.02 ), btScalar( 0.5)));
	m_shapes[BODYPART_RIGHT_LOW] = new btBoxShape(btVector3(btScalar(0.5 ), btScalar(0.02 ), btScalar( 0.5)));
	m_shapes[UNION_LEFT_UPP] = new btConeShape(0.1, 0.5);
	m_shapes[UNION_LEFT_LOW] = new btConeShape(0.1, 0.5);
	m_shapes[UNION_RIGHT_UPP] = new btConeShape(0.1, 0.5);
	m_shapes[UNION_RIGHT_LOW] = new btConeShape(0.1, 0.5);

	
	masas[MASS_COMP] = new btScalar(1000.0);
	masas[MASS_LEFT_LOW] = new btScalar(10.);
	masas[MASS_RIGHT_LOW] = new btScalar(10.);
	masas[MASS_LEFT_UPP] = new btScalar(10.);
	masas[MASS_RIGHT_UPP] = new btScalar(10.);

	origenes[MASS_COMP] = new btVector3(positionOffset);
	origenes[MASS_LEFT_UPP] = new btVector3(positionOffset + btVector3(-2.8, 0.61, 2.8));
	origenes[MASS_LEFT_LOW] = new btVector3(positionOffset + btVector3(-2.8, 0.61, -2.8));
	origenes[MASS_RIGHT_UPP] = new btVector3(positionOffset + btVector3(2.8, 0.61, 2.8));
	origenes[MASS_RIGHT_LOW] = new btVector3(positionOffset + btVector3(2.8, 0.61, -2.8));

	for (int i = 0; i < FORCE_COUNT; i++)
		fuerzas[i] = new double(0.0);
	

	btCompoundShape* base = new btCompoundShape();
	btTransform transformada;
	transformada.setIdentity();
	transformada.setOrigin(btVector3(0, 0.05, 0));
	base->addChildShape(transformada, m_shapes[BODYPART_BASE]);
	transformada.setIdentity();
	transformada.setOrigin(btVector3(2.8, 0.35, 2.8));
	base->addChildShape(transformada, m_shapes[UNION_RIGHT_UPP]);
	transformada.setIdentity();
	transformada.setOrigin(btVector3(-2.8, 0.35, 2.8));
	base->addChildShape(transformada, m_shapes[UNION_LEFT_UPP]);
	transformada.setIdentity();
	transformada.setOrigin(btVector3(-2.8, 0.35, -2.8));
	base->addChildShape(transformada, m_shapes[UNION_LEFT_LOW]);
	transformada.setIdentity();
	transformada.setOrigin(btVector3(2.8, 0.35, -2.8));
	base->addChildShape(transformada, m_shapes[UNION_RIGHT_LOW]);
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(*origenes[MASS_COMP]);
	btRigidBody* rigidBase = localCreateRigidBody(btScalar(10.), transform, base);
	m_shapes[BODYPART_COMP] = base;
	m_bodies[MASS_COMP] = rigidBase;


	transform.setIdentity();
	transform.setOrigin(*origenes[MASS_LEFT_UPP]);
	m_bodies[MASS_LEFT_UPP] = localCreateRigidBody(btScalar(0.5), transform, m_shapes[BODYPART_LEFT_UPP]);
	
	transform.setIdentity();
	transform.setOrigin(*origenes[MASS_LEFT_LOW]);
	m_bodies[MASS_LEFT_LOW] = localCreateRigidBody(btScalar(0.5), transform, m_shapes[BODYPART_LEFT_LOW]);

	transform.setIdentity();
	transform.setOrigin(*origenes[MASS_RIGHT_UPP]);
	m_bodies[MASS_RIGHT_UPP] = localCreateRigidBody(btScalar(0.5), transform, m_shapes[BODYPART_RIGHT_UPP]);

	transform.setIdentity();
	transform.setOrigin(*origenes[MASS_RIGHT_LOW]);
	m_bodies[MASS_RIGHT_LOW] = localCreateRigidBody(btScalar(0.5), transform, m_shapes[BODYPART_RIGHT_LOW]);


///////////////////////////// SETTING THE CONSTRAINTS /////////////////////////////////////////////7777
	
	btGeneric6DofSpringConstraint* joint;
	btTransform localA, localB;
	bool useLinearReferenceFrameA = true;
/// ******* SPINE HEAD ******** ///
	
	{
		localA.setIdentity(); localB.setIdentity();
		localA.setOrigin(btVector3(btScalar(2.8), btScalar(0.6), btScalar(2.8)));
		localB.setOrigin(btVector3(btScalar(0.), btScalar(-0.02), btScalar(0.)));
		joint = new btGeneric6DofSpringConstraint(*rigidBase, *m_bodies[MASS_RIGHT_UPP], localA, localB,useLinearReferenceFrameA);
		joint->setAngularLowerLimit(btVector3(-SIMD_HALF_PI * 0.1, -SIMD_HALF_PI * 0.0, -SIMD_HALF_PI * 0.1));
		joint->setAngularUpperLimit(btVector3(SIMD_HALF_PI * 0.1, SIMD_HALF_PI * 0.0, SIMD_HALF_PI * 0.1));
		joint->setLinearLowerLimit(btVector3(0., 0., 0.));
		joint->setLinearUpperLimit(btVector3(0., 0., 0.)); 
		m_joints[JOINT_RIGHT_UP] = joint;
		fisicas->getDynamicsWorld()->addConstraint(m_joints[JOINT_RIGHT_UP], true);


		localA.setIdentity(); localB.setIdentity();
		localA.setOrigin(btVector3(btScalar(-2.8), btScalar(0.6), btScalar(2.8)));
		localB.setOrigin(btVector3(btScalar(0.), btScalar(-0.02), btScalar(0.)));
		joint = new btGeneric6DofSpringConstraint(*rigidBase, *m_bodies[MASS_LEFT_UPP], localA, localB, useLinearReferenceFrameA);
		joint->setAngularLowerLimit(btVector3(-SIMD_HALF_PI * 0.1, -SIMD_HALF_PI * 0.0, -SIMD_HALF_PI * 0.1));
		joint->setAngularUpperLimit(btVector3(SIMD_HALF_PI * 0.1, SIMD_HALF_PI * 0.0, SIMD_HALF_PI * 0.1));
		joint->setLinearLowerLimit(btVector3(0., 0., 0.));
		joint->setLinearUpperLimit(btVector3(0., 0., 0.));
		m_joints[JOINT_LEFT_UP] = joint;
		fisicas->getDynamicsWorld()->addConstraint(m_joints[JOINT_LEFT_UP], true);

		localA.setIdentity(); localB.setIdentity();
		localA.setOrigin(btVector3(btScalar(-2.8), btScalar(0.6), btScalar(-2.8)));
		localB.setOrigin(btVector3(btScalar(0.), btScalar(-0.02), btScalar(0.)));
		joint = new btGeneric6DofSpringConstraint(*rigidBase, *m_bodies[MASS_LEFT_LOW], localA, localB, useLinearReferenceFrameA);
		joint->setAngularLowerLimit(btVector3(-SIMD_HALF_PI * 0.1, -SIMD_HALF_PI * 0.0, -SIMD_HALF_PI * 0.1));
		joint->setAngularUpperLimit(btVector3(SIMD_HALF_PI * 0.1, SIMD_HALF_PI * 0.0, SIMD_HALF_PI * 0.1));
		joint->setLinearLowerLimit(btVector3(0., 0., 0.));
		joint->setLinearUpperLimit(btVector3(0., 0., 0.));
		m_joints[JOINT_LEFT_DOWN] = joint;
		fisicas->getDynamicsWorld()->addConstraint(m_joints[JOINT_LEFT_DOWN], true);

		localA.setIdentity(); localB.setIdentity();
		localA.setOrigin(btVector3(btScalar(2.8), btScalar(0.6), btScalar(-2.8)));
		localB.setOrigin(btVector3(btScalar(0.), btScalar(-0.02), btScalar(0.)));
		joint = new btGeneric6DofSpringConstraint(*rigidBase, *m_bodies[MASS_RIGHT_LOW], localA, localB, useLinearReferenceFrameA);
		joint->setAngularLowerLimit(btVector3(-SIMD_HALF_PI * 0.1, -SIMD_HALF_PI * 0.0, -SIMD_HALF_PI * 0.1));
		joint->setAngularUpperLimit(btVector3(SIMD_HALF_PI * 0.1, SIMD_HALF_PI * 0.0, SIMD_HALF_PI * 0.1));
		joint->setLinearLowerLimit(btVector3(0., 0., 0.));
		joint->setLinearUpperLimit(btVector3(0., 0., 0.));
		m_joints[JOINT_RIGHT_DOWN] = joint;
		fisicas->getDynamicsWorld()->addConstraint(m_joints[JOINT_RIGHT_DOWN], true);

	}
	
}

Drone6DOF::~Drone6DOF()
{
	int i;

	// Remove all constraints
	for (i = 0; i < JOINT_COUNT; ++i)
	{
		fisicas->getDynamicsWorld()->removeConstraint(m_joints[i]);
		delete m_joints[i]; m_joints[i] = 0;
	}
	
	// Remove all shapes used to construct btCompound. Other shapes are removed by other destructor
	for (i = 0; i < 5; ++i)
	{
		delete m_shapes[i]; m_shapes[i] = 0;
	}
	// Remove all bodies
	for (i = 0; i < MASS_COUNT; ++i)
	{
		fisicas->getDynamicsWorld()->removeRigidBody(m_bodies[i]);
		delete m_bodies[i]->getMotionState();
		delete m_bodies[i]; m_bodies[i] = 0;
		delete masas[i]; masas[i] = 0;
		delete origenes[i];origenes[i] = 0;
	}
	for (i = 0; i < FORCE_COUNT; ++i)
	{
		delete fuerzas[i]; fuerzas[i] = 0;
	}
	
}


btRigidBody* Drone6DOF::localCreateRigidBody (btScalar mass, const btTransform& startTransform, btCollisionShape* shape)
{
	bool isDynamic = (mass != 0.f);
	btVector3 localInertia(0,0,0);
	if (isDynamic)
		shape->calculateLocalInertia(mass,localInertia);
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,shape,localInertia);
	rbInfo.m_additionalDamping = true;
	btRigidBody* body = new btRigidBody(rbInfo);
	body->setActivationState(DISABLE_DEACTIVATION);
	fisicas->add(shape, body);
	return body;
}



void Drone6DOF::init()
{
	btMatrix3x3 original = m_bodies[0]->getWorldTransform().getBasis();
	for (size_t i = 1; i < MASS_COUNT; i++)
	{
		btTransform aux = m_bodies[i]->getWorldTransform();
		aux.setBasis(original);
		m_bodies[i]->setWorldTransform(aux);
		
	}	
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

void Drone6DOF::setPIDActionId(const char * id)
{
	m_fuerza = id;
}

void Drone6DOF::updateBulletState(State * s, const Action * a, double dt)
{
	if (m_fuerza!=NULL)
	{
		*fuerzas[F1_1] = a->get(m_fuerza);
		*fuerzas[F1_2] = a->get(m_fuerza);
		*fuerzas[F1_3] = a->get(m_fuerza);
		*fuerzas[F1_4] = a->get(m_fuerza);

		*fuerzas[F2_1] = a->get(m_fuerza);
		*fuerzas[F2_2] = a->get(m_fuerza);
		*fuerzas[F2_3] = a->get(m_fuerza);
		*fuerzas[F2_4] = a->get(m_fuerza);

		*fuerzas[F3_1] = a->get(m_fuerza);
		*fuerzas[F3_2] = a->get(m_fuerza);
		*fuerzas[F3_3] = a->get(m_fuerza);
		*fuerzas[F3_4] = a->get(m_fuerza);

		*fuerzas[F4_1] = a->get(m_fuerza);
		*fuerzas[F4_2] = a->get(m_fuerza);
		*fuerzas[F4_3] = a->get(m_fuerza);
		*fuerzas[F4_4] = a->get(m_fuerza);
	}
	else {


		*fuerzas[F1_1] = a->get(m_f1_1Id);
		*fuerzas[F1_2] = a->get(m_f1_1Id);
		*fuerzas[F1_3] = a->get(m_f1_1Id);
		*fuerzas[F1_4] = a->get(m_f1_1Id);

		*fuerzas[F2_1] = a->get(m_f1_1Id);
		*fuerzas[F2_2] = a->get(m_f1_1Id);
		*fuerzas[F2_3] = a->get(m_f1_1Id);
		*fuerzas[F2_4] = a->get(m_f1_1Id);

		*fuerzas[F3_1] = a->get(m_f1_1Id);
		*fuerzas[F3_2] = a->get(m_f1_1Id);
		*fuerzas[F3_3] = a->get(m_f1_1Id);
		*fuerzas[F3_4] = a->get(m_f1_1Id);

		*fuerzas[F4_1] = a->get(m_f1_1Id);
		*fuerzas[F4_2] = a->get(m_f1_1Id);
		*fuerzas[F4_3] = a->get(m_f1_1Id);
		*fuerzas[F4_4] = a->get(m_f1_1Id);

		/*

		*fuerzas[F1_1] = a->get(m_f1_1Id);
		*fuerzas[F1_2] = a->get(m_f1_2Id);
		*fuerzas[F1_3] = a->get(m_f1_3Id);
		*fuerzas[F1_4] = a->get(m_f1_4Id);

		*fuerzas[F2_1] = a->get(m_f2_1Id);
		*fuerzas[F2_2] = a->get(m_f2_2Id);
		*fuerzas[F2_3] = a->get(m_f2_3Id);
		*fuerzas[F2_4] = a->get(m_f2_4Id);

		*fuerzas[F3_1] = a->get(m_f3_1Id);
		*fuerzas[F3_2] = a->get(m_f3_2Id);
		*fuerzas[F3_3] = a->get(m_f3_3Id);
		*fuerzas[F3_4] = a->get(m_f3_4Id);

		*fuerzas[F4_1] = a->get(m_f4_1Id);
		*fuerzas[F4_2] = a->get(m_f4_2Id);
		*fuerzas[F4_3] = a->get(m_f4_3Id);
		*fuerzas[F4_4] = a->get(m_f4_4Id);
		*/
	}
	
	int j = 0;
	for (size_t i = 1; i < MASS_COUNT; i++)
	{
		btVector3 relativeForce = btVector3(0., 1.0, 0.);
		btMatrix3x3& boxRot = m_bodies[i]->getWorldTransform().getBasis();
		
		//m_bodies[i]->activate(true);

		m_bodies[i]->applyForce(boxRot *(relativeForce**fuerzas[j]), btVector3(0.3, 0.0, 0.3));
		 
		j++;

		m_bodies[i]->applyForce(boxRot *(relativeForce**fuerzas[j]), btVector3(0.3, 0.0, -0.3));

		j++;

		m_bodies[i]->applyForce(boxRot *(relativeForce**fuerzas[j]), btVector3(-0.3, 0.0, 0.3));

		j++;

		m_bodies[i]->applyForce(boxRot *(relativeForce**fuerzas[j]), btVector3(-0.3, 0.0, -0.3));

		j++;


	}


}
void Drone6DOF::reset(State * s)
{
	double altura = s->get("base-y");
	s->set("d-error-z", 0);
	btTransform bodyTransform;
	btQuaternion orientation;
	btVector3 zeroVector(0, 0, 0);
	for (size_t i = 0; i < MASS_COUNT; i++)
	{
		m_bodies[i]->clearForces();
		m_bodies[i]->setLinearVelocity(zeroVector);
		m_bodies[i]->setAngularVelocity(zeroVector);

		bodyTransform = m_bodies[i]->getWorldTransform();
		bodyTransform.setOrigin(*origenes[i]+btVector3(0.0,altura,0.0));
		orientation.setEuler(0.0, 0.0, 0.0);
		bodyTransform.setRotation(orientation);

		m_bodies[i]->setWorldTransform(bodyTransform);
		m_bodies[i]->getMotionState()->setWorldTransform(bodyTransform);

		btVector3 localInertia(0, 0, 0);
		m_bodies[i]->getCollisionShape()->calculateLocalInertia(*masas[i], localInertia);
	}
	
	updateState(s);
}
void Drone6DOF::updateState(State * s)
{
	btTransform transform = m_bodies[MASS_COMP]->getWorldTransform();
	
	s->set(m_xId, transform.getOrigin().x());
	s->set(m_yId, transform.getOrigin().y());
	s->set(m_zId, transform.getOrigin().z());

	s->set(m_error, altura- transform.getOrigin().y() );
	s->set("errorX", origenes[0]->x()- transform.getOrigin().x());
	s->set("errorY", origenes[0]->z() - transform.getOrigin().z());


	btMatrix3x3& boxRot = transform.getBasis();


	btScalar x,y,z; 
	boxRot.getEulerZYX(z, y, x);

	s->set(m_rotXId, x);
	s->set(m_rotYId, y);
	s->set(m_rotZId, z);

	btVector3 velocity = m_bodies[MASS_COMP]->getAngularVelocity();
	s->set(m_angularVXId, velocity.x());
	s->set(m_angularVYId, velocity.y());
	s->set(m_angularVZId, velocity.z());

	velocity = m_bodies[MASS_COMP]->getLinearVelocity();
	s->set(m_linearVXId, velocity.x());
	s->set(m_linearVYId, velocity.y());
	s->set(m_linearVZId, velocity.z());



	for (size_t i = 1; i < MASS_COUNT; i++)
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


