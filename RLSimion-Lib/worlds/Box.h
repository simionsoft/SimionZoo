#pragma once
#include "BulletBody.h"

class Box : public BulletBody {
public:
	Box(double mass, const btVector3& pos, btCollisionShape* shape, bool moving_obj) : BulletBody(mass, pos, shape, moving_obj)
	{
	}
	virtual ~Box() {}
	void setRelativeVariables(CState* s, int idX, int idY, double targetX, double targetY);
};