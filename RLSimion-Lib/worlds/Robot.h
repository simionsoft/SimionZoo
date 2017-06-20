#pragma once
#include "BulletBody.h"

class Robot : public BulletBody {
public:
	Robot(double mass, const btVector3& pos, btCollisionShape* shape, bool moving_obj) : BulletBody(mass, pos, shape, moving_obj)
	{
	}
	virtual ~Robot() {}
	void setRelativeVariables(CState* s, int idX, int idY, double valX, double valY);
};