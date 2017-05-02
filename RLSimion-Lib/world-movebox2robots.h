#pragma once

#include "world.h"
#include "reward.h"
#define ATTRIBUTE_ALIGNED16(a)
#include "../3rd-party/bullet3-2.86/src/btBulletDynamicsCommon.h"
class SimpleOpenGL3App;
class BulletBuilder;
class BulletBody;


//Move box with 2 robots
class CMoveBox2Robots : public CDynamicModel
{
	/// All-Simulation variables
	double MASS_ROBOT;
	double MASS_BOX;
	double MASS_TARGET;
	double MASS_GROUND;

	double o_distBr1X;
	double o_distBr1Y;
	double o_distBr2X;
	double o_distBr2Y;
	double o_distBtX;
	double o_distBtY;

	/// Episode variables
	int m_rob1_X, m_rob1_Y;
	int m_rob2_X, m_rob2_Y;
	int m_box_X, m_box_Y;
	int m_D_Br1X, m_D_Br1Y;
	int m_D_Br2X, m_D_Br2Y;
	int m_D_BtX, m_D_BtY;
	int m_theta_r1;
	int m_theta_r2;
	

	// Action variables
	int m_linear_vel_r1;
	int m_omega_r1;
	int m_linear_vel_r2;
	int m_omega_r2;

	///Graphic initialization
	SimpleOpenGL3App* window_app;
	BulletBuilder* rob2Builder;

	///Bullet bodies init
	BulletBody *m_Ground;
	BulletBody *m_Robot1;
	BulletBody *m_Robot2;
	BulletBody *m_Box;
	BulletBody *m_Target;


public:
	CMoveBox2Robots(CConfigNode* pParameters);
	virtual ~CMoveBox2Robots() = default;

	void reset(CState *s);
	void executeAction(CState *s, const CAction *a, double dt);
};

class CMoveBox2RobotsReward : public IRewardComponent
{
public:
	double getReward(const CState *s, const CAction *a, const CState *s_p);
	const char* getName() { return "reward"; }
	double getMin();
	double getMax();
};