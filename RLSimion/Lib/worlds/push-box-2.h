#pragma once

#include "world.h"

class BulletPhysics;


//Move box with 2 robots
class PushBox2 : public DynamicModel
{
	/// State variables
	size_t m_target_X, m_target_Y;
	size_t m_rob1_X, m_rob1_Y;
	size_t m_rob2_X, m_rob2_Y;
	size_t m_box_X, m_box_Y, m_boxTheta;
	size_t m_D_Br1X, m_D_Br1Y;
	size_t m_D_Br2X, m_D_Br2Y;
	size_t m_D_BtX, m_D_BtY;
	size_t m_theta_r1;
	size_t m_theta_r2;
	
	BulletPhysics* m_pBulletPhysics;

public:
	PushBox2(ConfigNode* pParameters);
	virtual ~PushBox2();

	void reset(State *s);
	void executeAction(State *s, const Action *a, double dt);
};
