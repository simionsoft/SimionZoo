#pragma once
#include "world.h"
#include "../3rd-party/bullet3-2.86/src/btBulletDynamicsCommon.h"

class BulletBody {

public:
	BulletBody(double mass, double xPos, double yPos, double zPos, btCollisionShape* shape, bool moving_obj);
	virtual ~BulletBody() { }

	btRigidBody* BulletBody::getBody();
	btCollisionShape* BulletBody::getShape();
	btTransform BulletBody::getTransform();
	void updateResetVariables(CState* s,bool isBox, double originX, double originY, int idX, int idY);
	double updateRobotMovement(const CAction *a, CState *s, char *omega, char *vel, int theta, double dt);
	btTransform setAbsoluteActionVariables(CState* s, double idX, double idY);
	void setRelativeActionVariables(CState* s, int idX, int idY, bool isBox, double targetX, double targetY, double valX = 0.0, double valY = 0.0);

protected:
	btRigidBody* m_pBody;
	btCollisionShape* m_shape;
	btTransform m_transform;
	btScalar m_mass;
	btVector3 m_localInertia;
};