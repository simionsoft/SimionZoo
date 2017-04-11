#pragma once
#include "../3rd-party/bullet3-2.86/src/btBulletDynamicsCommon.h"

class BulletBody {

public:
	BulletBody(double mass, double xPos, double yPos, double zPos, btCollisionShape* shape, bool moving_obj);
	virtual ~BulletBody() { }

	btRigidBody* BulletBody::getBody();
	btCollisionShape* BulletBody::getShape();
	btTransform BulletBody::getTransform();

protected:
	btRigidBody* m_pBody;
	btCollisionShape* m_shape;
	btTransform m_transform;
	btScalar m_mass;
	btVector3 m_localInertia;
};