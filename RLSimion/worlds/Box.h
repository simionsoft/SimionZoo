#pragma once
#include "BulletBody.h"

class Box : public BulletBody {
public:
	Box(double mass, const btVector3& pos, btCollisionShape* shape) : BulletBody(mass, pos, shape, 0)
	{
	}
	virtual ~Box() {}

};

class StaticObject : public BulletBody
{
public:
	StaticObject(const btVector3& pos, btCollisionShape* shape) : BulletBody(0, pos, shape, 0)
	{
	}
	virtual ~StaticObject() {}
};

class KinematicObject : public BulletBody
{
public:
	KinematicObject(double mass, const btVector3& pos, btCollisionShape* shape) : BulletBody(mass, pos, shape, btCollisionObject::CF_KINEMATIC_OBJECT)
	{
	}
	virtual ~KinematicObject() {}
};