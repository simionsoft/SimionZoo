#pragma once
#include "BulletBody.h"
#include "BulletPhysics.h"
#include <string>
using namespace std;


class Drone6DOF:public BulletBody
{
	
	enum
	{
		BODYPART_BASE = 0,
		UNION_LEFT_UPP,
		UNION_LEFT_LOW,
		UNION_RIGHT_UPP,
		UNION_RIGHT_LOW,
		BODYPART_COMP,
		BODYPART_LEFT_UPP,
		BODYPART_LEFT_LOW,
		BODYPART_RIGHT_UPP,
		BODYPART_RIGHT_LOW,
		BODYPART_COUNT
	};
	enum
	{
		MASS_COMP = 0,
		MASS_LEFT_UPP,
		MASS_LEFT_LOW,
		MASS_RIGHT_UPP,
		MASS_RIGHT_LOW,
		MASS_COUNT
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
		JOINT_LEFT_UP=0,
		JOINT_LEFT_DOWN,
		JOINT_RIGHT_UP,
		JOINT_RIGHT_DOWN,
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

	const char* m_error;
	const char* m_fuerza = NULL;

	const char* m_x1Id;
	const char* m_y1Id;
	const char* m_z1Id;
	const char* m_rotX1Id;
	const char* m_rotY1Id;
	const char* m_rotZ1Id;
	const char* m_linearVX1Id;
	const char* m_linearVY1Id;
	const char* m_linearVZ1Id;
	const char* m_angularVX1Id;
	const char* m_angularVY1Id;
	const char* m_angularVZ1Id;

	const char* m_x2Id;
	const char* m_y2Id;
	const char* m_z2Id;
	const char* m_rotX2Id;
	const char* m_rotY2Id;
	const char* m_rotZ2Id;
	const char* m_linearVX2Id;
	const char* m_linearVY2Id;
	const char* m_linearVZ2Id;
	const char* m_angularVX2Id;
	const char* m_angularVY2Id;
	const char* m_angularVZ2Id;

	const char* m_x3Id;
	const char* m_y3Id;
	const char* m_z3Id;
	const char* m_rotX3Id;
	const char* m_rotY3Id;
	const char* m_rotZ3Id;
	const char* m_linearVX3Id;
	const char* m_linearVY3Id;
	const char* m_linearVZ3Id;
	const char* m_angularVX3Id;
	const char* m_angularVY3Id;
	const char* m_angularVZ3Id;

	const char* m_x4Id;
	const char* m_y4Id;
	const char* m_z4Id;
	const char* m_rotX4Id;
	const char* m_rotY4Id;
	const char* m_rotZ4Id;
	const char* m_linearVX4Id;
	const char* m_linearVY4Id;
	const char* m_linearVZ4Id;
	const char* m_angularVX4Id;
	const char* m_angularVY4Id;
	const char* m_angularVZ4Id;


	BulletPhysics* fisicas;
	btCollisionShape* m_shapes[BODYPART_COUNT];
	btRigidBody* m_bodies[MASS_COUNT];
	btGeneric6DofSpringConstraint* m_joints[JOINT_COUNT];
	double* fuerzas[FORCE_COUNT];
	btScalar* masas[MASS_COUNT];
	btVector3* origenes[MASS_COUNT];
	
	btRigidBody* localCreateRigidBody (btScalar mass, const btTransform& startTransform, btCollisionShape* shape);

public:
	static constexpr double altura=10.0;
	Drone6DOF (BulletPhysics* ownerWorld,
				const btVector3& positionOffset);
	void subir();
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

		m_f2_1Id = f11;
		m_f2_2Id = f12;
		m_f2_3Id = f13;
		m_f2_4Id = f14;

		m_f3_1Id = f11;
		m_f3_2Id = f12;
		m_f3_3Id = f13;
		m_f3_4Id = f14;

		m_f4_1Id = f11;
		m_f4_2Id = f12;
		m_f4_3Id = f13;
		m_f4_4Id = f14;
		
	}
	void setAbsoluteStateVarIds(const char * xId, const char * yId, const char * zId, const char * rotXId, const char * rotYId, const char * rotZId, const char * angularVXId, const char * angularVYId, const char * angularVZId, const char * linearVXId, const char * linearVYId, const char * linearVZId,
		const char * x1Id, const char * y1Id, const char * z1Id, const char * rotX1Id, const char * rotY1Id, const char * rotZ1Id, const char * angularVX1Id, const char * angularVY1Id, const char * angularVZ1Id, const char * linearVX1Id, const char * linearVY1Id, const char * linearVZ1Id,
		const char * x2Id, const char * y2Id, const char * z2Id, const char * rotX2Id, const char * rotY2Id, const char * rotZ2Id, const char * angularVX2Id, const char * angularVY2Id, const char * angularVZ2Id, const char * linearVX2Id, const char * linearVY2Id, const char * linearVZ2Id,
		const char * x3Id, const char * y3Id, const char * z3Id, const char * rotX3Id, const char * rotY3Id, const char * rotZ3Id, const char * angularVX3Id, const char * angularVY3Id, const char * angularVZ3Id, const char * linearVX3Id, const char * linearVY3Id, const char * linearVZ3Id,
		const char * x4Id, const char * y4Id, const char * z4Id, const char * rotX4Id, const char * rotY4Id, const char * rotZ4Id, const char * angularVX4Id, const char * angularVY4Id, const char * angularVZ4Id, const char * linearVX4Id, const char * linearVY4Id, const char * linearVZ4Id)override;//override error
	void setErrorStateVarId(const char* id);
	void setPIDActionId(const char* id);
	void updateBulletState(State* s, const Action* a, double dt) override;
	void reset(State* s) override;
	void updateState(State* s) override;
	virtual ~Drone6DOF ();
};




