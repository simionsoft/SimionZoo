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
	int m_xId = -1;
	int m_yId = -1;
	int m_thetaId = -1;

	int m_relXId = -1;
	int m_relYId = -1;
	int m_refXId = -1;
	int m_refYId = -1;

	bool bSetAbsStateVars() { return m_bSetAbsStateVars; }
	bool bSetRelStateVars() { return m_bSetRelStateVars; }

	//only to be called by Rope subclass
	BulletBody() {}

	//protected because we want to force the use of subclasses: StaticObject, Robot, BulletBox, KinematicObject...
	BulletBody(double mass, const btVector3& pos, btCollisionShape* shape, int objType= 0);
public:
	virtual ~BulletBody() { }

	void setAbsoluteStateVarIds(int xId, int yId, int thetaId);
	void setRelativeStateVarIds(int relXId, int relYId, int refXId, int refYId);
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