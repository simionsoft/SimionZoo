#pragma once
#include "BulletBody.h"
#include "BulletPhysics.h"
#include <string>
using namespace std;


class Drone6DOF : public BulletBody
{
	
	enum
	{
		BASE = 0,
		DRONE_1,
		DRONE_2,
		DRONE_3,
		DRONE_4,
		DRONE_ELEMENT_COUNT
	};

	enum
	{
		F1_1 = 0,
		F1_2,
		F1_3,
		F1_4,
		F2_1,
		F2_2,
		F2_3,
		F2_4,
		F3_1,
		F3_2,
		F3_3,
		F3_4,
		F4_1,
		F4_2,
		F4_3,
		F4_4,
		FORCE_COUNT
	};


	enum
	{
		JOINT_DRONE_1 = 0,
		JOINT_DRONE_2,
		JOINT_DRONE_3,
		JOINT_DRONE_4,
		JOINT_COUNT
	};
	
	const char* m_f1_1Id;
	const char* m_f1_2Id;
	const char* m_f1_3Id;
	const char* m_f1_4Id;
	const char* m_f2_1Id;
	const char* m_f2_2Id;
	const char* m_f2_3Id;
	const char* m_f2_4Id;
	const char* m_f3_1Id;
	const char* m_f3_2Id;
	const char* m_f3_3Id;
	const char* m_f3_4Id;
	const char* m_f4_1Id;
	const char* m_f4_2Id;
	const char* m_f4_3Id;
	const char* m_f4_4Id;

	static const int m_numDrones = 4;

	BulletPhysics* m_physics;
	btCollisionShape* m_shapes[DRONE_ELEMENT_COUNT];
	btRigidBody* m_bodies[DRONE_ELEMENT_COUNT];
	btGeneric6DofSpringConstraint* m_joints[m_numDrones];
	double m_forces[FORCE_COUNT];
	btScalar m_masses[DRONE_ELEMENT_COUNT];
	btVector3 m_origins[DRONE_ELEMENT_COUNT];
	
	btRigidBody* localCreateRigidBody (btScalar mass, const btTransform& startTransform, btCollisionShape* shape);

	const double m_droneRelPosY = 3.0;
	const double m_baseHalfWidth = 3.0;
	const double m_baseHalfHeight = 0.1;
	const double m_jointPositionOffset = m_baseHalfWidth - 0.2;
	const double m_droneHeight = 0.02;
	const double m_droneWidth = 0.5;
	const double m_baseMass = 10.0;
	const double m_droneMass = 1.0;

public:
	Drone6DOF (BulletPhysics* ownerWorld);
	
	void init();
	void setActionIds(const char* f11, const char* f12, const char* f13, const char* f14,
		const char* f21, const char* f22, const char* f23, const char* f24, 
		const char* f31, const char* f32, const char* f33, const char* f34, 
		const char* f41, const char* f42, const char* f43, const char* f44) 
	{ 
		m_f1_1Id = f11;
		m_f1_2Id = f12;
		m_f1_3Id = f13;
		m_f1_4Id = f14;

		m_f2_1Id = f21;
		m_f2_2Id = f22;
		m_f2_3Id = f23;
		m_f2_4Id = f24;

		m_f3_1Id = f31;
		m_f3_2Id = f32;
		m_f3_3Id = f33;
		m_f3_4Id = f34;

		m_f4_1Id = f41;
		m_f4_2Id = f42;
		m_f4_3Id = f43;
		m_f4_4Id = f44;
		
	}
	void setAbsoluteStateVarIds(const char * xId, const char * yId, const char * zId,
		const char * rotXId, const char * rotYId, const char * rotZId,
		const char * angularVXId, const char * angularVYId, const char * angularVZId,
		const char * linearVXId, const char * linearVYId, const char * linearVZId);

	void updateBulletState(State* s, const Action* a, double dt) override;
	void reset(State* s) override;
	void updateState(State* s) override;
	virtual ~Drone6DOF ();
};




