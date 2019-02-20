#pragma once

#include "world.h"
class BulletPhysics;


//Move box with 1 robot
class PushBox1 : public DynamicModel
{
	/// State variables
	size_t m_target_X, m_target_Y;
	size_t m_rob1_X, m_rob1_Y;
	size_t m_box_X, m_box_Y;
	size_t m_D_BrX, m_D_BrY;
	size_t m_D_BtX, m_D_BtY;
	size_t m_theta;
	size_t m_boxTheta;

	///Graphic initialization
	BulletPhysics* m_pBulletPhysics;

public:

	PushBox1(ConfigNode* pParameters);
	virtual ~PushBox1();

	void reset(State *s);
	void executeAction(State *s, const Action *a, double dt);

};