#pragma once

#include "world.h"
class BulletPhysics;


//Move box with 1 robot
class CPushBox1 : public CDynamicModel
{
	/// State variables
	int m_target_X, m_target_Y;
	int m_rob1_X, m_rob1_Y;
	int m_box_X, m_box_Y;
	int m_D_BrX, m_D_BrY;
	int m_D_BtX, m_D_BtY;
	int m_theta;
	int m_boxTheta;

	///Graphic initialization
	BulletPhysics* m_pBulletPhysics;

public:

	CPushBox1(CConfigNode* pParameters);
	virtual ~CPushBox1();

	void reset(CState *s);
	void executeAction(CState *s, const CAction *a, double dt);

};