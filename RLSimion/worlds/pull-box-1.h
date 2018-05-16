#pragma once

#include "world.h"

class BulletPhysics;


//Move box with 1 robot and 1 rope
class PullBox1 : public DynamicModel
{

	/// State variables
	size_t m_target_X, m_target_Y;
	size_t m_state_X, m_state_Y;
	size_t m_rob1_X, m_rob1_Y;
	size_t m_box_X, m_box_Y;
	size_t m_D_Br1X, m_D_Br1Y;;
	size_t m_D_BtX, m_D_BtY;
	size_t m_theta_r1;
	size_t m_boxTheta;

	BulletPhysics* m_pBulletPhysics;

public:
	PullBox1(ConfigNode* pParameters);
	virtual ~PullBox1();

	void reset(State *s);
	void executeAction(State *s, const Action *a, double dt);
};
