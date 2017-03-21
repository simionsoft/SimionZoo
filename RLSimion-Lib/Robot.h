#pragma once
class btRigidBody;
class btVector3;
class btDefaultMotionState;
class btCollisionShape;


class Robot {
public:
	Robot(double mass, btDefaultMotionState* motionState, btCollisionShape* shape, btVector3 inertia);
	virtual ~Robot() { }

	void setAppliedForce(btVector3 &applied_force);
	void getAppliedForce(btVector3 &applied_force);
	btRigidBody* Robot::getBody();

protected:
	btVector3 m_force;
	btRigidBody* m_pBody;
};