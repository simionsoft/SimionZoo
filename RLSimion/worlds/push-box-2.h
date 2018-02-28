#pragma once

#include "world.h"

class BulletPhysics;


//Move box with 2 robots
class CPushBox2 : public CDynamicModel
{
	/// State variables
	int m_target_X, m_target_Y;
	int m_rob1_X, m_rob1_Y;
	int m_rob2_X, m_rob2_Y;
	int m_box_X, m_box_Y, m_boxTheta;
	int m_D_Br1X, m_D_Br1Y;
	int m_D_Br2X, m_D_Br2Y;
	int m_D_BtX, m_D_BtY;
	int m_theta_r1;
	int m_theta_r2;
	
	BulletPhysics* m_pBulletPhysics;

public:
	CPushBox2(CConfigNode* pParameters);
	virtual ~CPushBox2();

	void reset(CState *s);
	void executeAction(CState *s, const CAction *a, double dt);
};
