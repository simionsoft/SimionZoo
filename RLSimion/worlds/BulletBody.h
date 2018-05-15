#pragma once
#include "world.h"
#include "../../3rd-party/bullet3-2.86/src/btBulletDynamicsCommon.h"

class BulletBody
{
	btVector3 m_localInertia;
	bool m_bSetRelStateVars = false;
	bool m_bSetAbsStateVars = false;

	double m_originX = 0.0;
	double m_originY = 0.0;
	double m_originZ = 0.0;
	double m_originTheta = 0.0;
protected:
	size_t m_xId;
	size_t m_yId;
	size_t m_thetaId;

	size_t m_relXId;
	size_t m_relYId;
	size_t m_refXId;
	size_t m_refYId;

	bool bSetAbsStateVars() { return m_bSetAbsStateVars; }
	bool bSetRelStateVars() { return m_bSetRelStateVars; }

	//only to be called by Rope subclass
	BulletBody() {}

	//protected because we want to force the use of subclasses: StaticObject, Robot, BulletBox, KinematicObject...
	BulletBody(double mass, const btVector3& pos, btCollisionShape* shape, int objType= 0);
public:
	virtual ~BulletBody() { }

	void setAbsoluteStateVarIds(size_t xId, size_t yId, size_t thetaId);
	void setRelativeStateVarIds(size_t relXId, size_t relYId, size_t refXId, size_t refYId);
	void setOrigin(double x, double y, double theta);

	virtual void reset(State* s);
	virtual void updateState(State* s);
	virtual void updateBulletState(State *s, const Action *a, double dt) {}

	btRigidBody* BulletBody::getBody();
	btCollisionShape* BulletBody::getShape();
	virtual bool bIsRigidBody() { return true; }
protected:
	btRigidBody* m_pBody= 0;
	btCollisionShape* m_shape= 0;

	btScalar m_mass;

	virtual void updateYawState(State* s);
	void resetInertia();
};