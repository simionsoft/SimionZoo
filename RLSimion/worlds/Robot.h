#pragma once
#include "BulletBody.h"

class Robot : public BulletBody
{
	const char* m_vId;
	const char* m_omegaId;

	//in robots, this is calculated manually, so we must store it for a later call of updateYawState()
	double m_theta= 0.0;
public:
	Robot(double mass, const btVector3& pos, btCollisionShape* shape);
	virtual ~Robot() {}

	virtual void updateBulletState(State *s, const Action *a, double dt);

	void setActionIds(const char* v, const char* omega) { m_vId = v; m_omegaId = omega; }

	virtual void updateYawState(State* s);
};