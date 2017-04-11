#pragma once

#include "world.h"
#include "reward.h"
#define ATTRIBUTE_ALIGNED16(a)
#include "../3rd-party/bullet3-2.86/src/btBulletDynamicsCommon.h"

class Robot;
class BulletBody;
class SimpleOpenGL3App;
class GraphicSettings;

//Move box with 2 robots
class COnlyRobot : public CDynamicModel
{
	/// All-Simulation variables
	double GRAVITY;
	double MASS_ROBOT;
	double MASS_GROUND;

	/// Episode variables
	int rob1_X, rob1_Y;

	// Action variables
	int rob1_forceX, rob1_forceY;

	///inicialización
	SimpleOpenGL3App* app;
	GraphicSettings* robotOnlyGraphs;

	BulletBody* robot_bb;
	BulletBody* ground_bb;

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