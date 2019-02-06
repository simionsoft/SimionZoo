#pragma once
#include "BulletBody.h"

class BulletBox : public BulletBody
{
public:
	BulletBox(double mass, const btVector3& pos, btCollisionShape* shape) : BulletBody(mass, pos, shape, 0)
	{
	}
	virtual ~BulletBox() {}

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