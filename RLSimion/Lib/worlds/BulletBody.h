#pragma once
#include "world.h"
#include "../../../3rd-party/bullet3-2.86/src/btBulletDynamicsCommon.h"

class BulletBody
{
	btVector3 m_localInertia;
	bool m_bRelVariablesSet = false;
	bool m_bAbsVariablesSet = false;
	bool m_bAngleSet = false;

	double m_originX = 0.0;
	double m_originY = 0.0;
	double m_originZ = 0.0;
	double m_originTheta = 0.0;
	/* añadidas para drone6dof*/
	double m_rotX = 0.0;
	double m_rotY = 0.0;
	double m_rotz = 0.0;
	
protected:
	const char* m_xId;
	const char* m_yId;
	const char* m_thetaId;
	/* añadidas para drone6dof*/
	const char* m_zId;
	const char* m_rotXId;
	const char* m_rotYId;
	const char* m_rotZId;
	const char* m_linearVXId;
	const char* m_linearVYId;
	const char* m_linearVZId;
	const char* m_angularVXId;
	const char* m_angularVYId;
	const char* m_angularVZId;

	const char* m_relXId;
	const char* m_relYId;
	const char* m_refXId;
	const char* m_refYId;

	bool areAbsVariablesSet() { return m_bAbsVariablesSet; }
	bool areRelVariablesSet() { return m_bRelVariablesSet; }
	bool isAngleSet() const { return m_bAngleSet; }

	//only to be called by Rope subclass
	BulletBody() {}

	//protected because we want to force the use of subclasses: StaticObject, Robot, BulletBox, KinematicObject...
	BulletBody(double mass, const btVector3& pos, btCollisionShape* shape, int objType= 0);

	BulletBody(const btVector3& pos);
public:
	virtual ~BulletBody() { }
	
	void setAbsoluteStateVarIds(const char* xId, const char* yId, const char* thetaId);
	void setAbsoluteStateVarIds(const char* xId, const char* yId);

	virtual void setAbsoluteStateVarIds(const char * xId, const char * yId, const char * zId, const char * rotXId, const char * rotYId, const char * rotZId, const char * angularVXId, const char * angularVYId, const char * angularVZId, const char * linearVXId, const char * linearVYId, const char * linearVZId,
		const char * x1Id, const char * y1Id, const char * z1Id, const char * rotX1Id, const char * rotY1Id, const char * rotZ1Id, const char * angularVX1Id, const char * angularVY1Id, const char * angularVZ1Id, const char * linearVX1Id, const char * linearVY1Id, const char * linearVZ1Id,
		const char * x2Id, const char * y2Id, const char * z2Id, const char * rotX2Id, const char * rotY2Id, const char * rotZ2Id, const char * angularVX2Id, const char * angularVY2Id, const char * angularVZ2Id, const char * linearVX2Id, const char * linearVY2Id, const char * linearVZ2Id,
		const char * x3Id, const char * y3Id, const char * z3Id, const char * rotX3Id, const char * rotY3Id, const char * rotZ3Id, const char * angularVX3Id, const char * angularVY3Id, const char * angularVZ3Id, const char * linearVX3Id, const char * linearVY3Id, const char * linearVZ3Id,
		const char * x4Id, const char * y4Id, const char * z4Id, const char * rotX4Id, const char * rotY4Id, const char * rotZ4Id, const char * angularVX4Id, const char * angularVY4Id, const char * angularVZ4Id, const char * linearVX4Id, const char * linearVY4Id, const char * linearVZ4Id);
	
	void setRelativeStateVarIds(const char* relXId, const char* relYId, const char* refXId, const char* refYId);
	void setOrigin(double x, double y, double theta);


	virtual void reset(State* s);
	virtual void updateState(State* s);
	virtual void updateBulletState(State *s, const Action *a, double dt) {}

	btRigidBody* getBody();
	btCollisionShape* getShape();
	virtual bool bIsRigidBody() { return true; }
protected:
	btRigidBody* m_pBody= 0;
	btCollisionShape* m_shape= 0;

	btScalar m_mass;

	virtual void updateYawState(State* s);
	void resetInertia();
};