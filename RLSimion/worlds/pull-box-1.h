#pragma once

#include "world.h"

class BulletPhysics;


//Move box with 1 robot and 1 rope
class PullBox1 : public DynamicModel
{

	/// State variables
	int m_target_X, m_target_Y;
	int m_state_X, m_state_Y;
	int m_rob1_X, m_rob1_Y;
	int m_box_X, m_box_Y;
	int m_D_Br1X, m_D_Br1Y;;
	int m_D_BtX, m_D_BtY;
	int m_theta_r1;
	int m_boxTheta;

	BulletPhysics* m_pBulletPhysics;

public:
	PullBox1(ConfigNode* pParameters);
	virtual ~PullBox1();

	void reset(State *s);
	void executeAction(State *s, const Action *a, double dt);
};
