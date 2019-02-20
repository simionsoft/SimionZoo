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
protected:
	const char* m_xId;
	const char* m_yId;
	const char* m_thetaId;

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
public:
	virtual ~BulletBody() { }

	void setAbsoluteStateVarIds(const char* xId, const char* yId, const char* thetaId);
	void setAbsoluteStateVarIds(const char* xId, const char* yId);
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