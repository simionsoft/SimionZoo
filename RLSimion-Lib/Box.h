#pragma once
class btRigidBody;
class btVector3;
struct btDefaultMotionState;
class btCollisionShape;


class Box {
public:
	Box(double mass, btDefaultMotionState* motionState, btCollisionShape* shape, btVector3& inertia);
	virtual ~Box() { }

	btRigidBody* Box::getBody();

protected:
	btRigidBody* m_pBody;
};