#pragma once

#include "world.h"
#include "reward.h"
#define ATTRIBUTE_ALIGNED16(a)
#include "../3rd-party/bullet3-2.86/src/btBulletDynamicsCommon.h"

class Robot;
class BulletBody;
class SimpleOpenGL3App;
class BulletViewer;

//Move box with 2 robots
class COnlyRobot : public CDynamicModel
{
	/// All-Simulation variables
	double MASS_TARGET;
	double MASS_ROBOT;
	double MASS_GROUND;

	/// State variables
	int m_rob1_X, m_rob1_Y;
	int m_omega;
	int m_theta;

	// Action variables
	int m_linear_vel;

	///inicialización
	SimpleOpenGL3App* app;
	BulletViewer* robotOnlyGraphs;

	BulletBody* robot_bb;
	BulletBody* ground_bb;
	BulletBody* target_bb;

public:
	COnlyRobot(CConfigNode* pParameters);
	virtual ~COnlyRobot();

	void reset(CState *s);
	void executeAction(CState *s, const CAction *a, double dt);

};

class COnlyRobotReward : public IRewardComponent
{
public:
	double getReward(const CState *s, const CAction *a, const CState *s_p);
	const char* getName() { return "reward"; }
	double getMin();
	double getMax();
};