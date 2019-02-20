#pragma once

#include "world.h"

class BulletPhysics;



//Move box with 2 robots
class PullBox2 : public DynamicModel
{
	/// State variables
	size_t m_target_X, m_target_Y;
	size_t m_state_X, m_state_Y;
	size_t m_rob1_X, m_rob1_Y;
	size_t m_rob2_X, m_rob2_Y;
	size_t m_box_X, m_box_Y;
	size_t m_D_Br1X, m_D_Br1Y;
	size_t m_D_Br2X, m_D_Br2Y;
	size_t m_D_BtX, m_D_BtY;
	size_t m_theta_r1;
	size_t m_theta_r2;
	size_t m_boxTheta;

	BulletPhysics* m_pBulletPhysics;

public:
	PullBox2(ConfigNode* pParameters);
	virtual ~PullBox2();

	void reset(State *s);
	void executeAction(State *s, const Action *a, double dt);
};
