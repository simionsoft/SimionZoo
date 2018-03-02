#pragma once

#include "world.h"

class BulletPhysics;


//Move box with 1 robot and 1 rope
class CPullBox1 : public CDynamicModel
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
	CPullBox1(CConfigNode* pParameters);
	virtual ~CPullBox1();

	void reset(CState *s);
	void executeAction(CState *s, const CAction *a, double dt);
};
