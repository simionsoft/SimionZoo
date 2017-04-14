#pragma once

#include "world.h"
#include "reward.h"
#define ATTRIBUTE_ALIGNED16(a)
#include "../3rd-party/bullet3-2.86/src/btBulletDynamicsCommon.h"
class SimpleOpenGL3App;
class BulletBuilder;
class BulletBody;

//Move box with 2 robots
class CMoveBoxOneRobot : public CDynamicModel
{
	/// All-Simulation variables
	double MASS_ROBOT;
	double MASS_BOX;
	double MASS_TARGET;
	double MASS_GROUND;

	/// Episode variables
	int m_rob1_X, m_rob1_Y;
	int m_box_X, m_box_Y;
	int m_theta;

	// Action variables
	int m_linear_vel;
	int m_omega;

	///Graphic initialization
	SimpleOpenGL3App* app;
	BulletBuilder* rBoxBuilder;

	///Bullet bodies init
	BulletBody *m_Ground;
	BulletBody *m_Robot;
	BulletBody *m_Box;
	BulletBody *m_Target;

public:

	CMoveBoxOneRobot(CConfigNode* pParameters);
	virtual ~CMoveBoxOneRobot();

	void reset(CState *s);
	void executeAction(CState *s, const CAction *a, double dt);

};

class CMoveBoxOneRobotReward : public IRewardComponent
{
public:
	double getReward(const CState *s, const CAction *a, const CState *s_p);
	const char* getName() { return "reward"; }
	double getMin();
	double getMax();
};