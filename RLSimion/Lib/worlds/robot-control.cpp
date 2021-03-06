/*
	SimionZoo: A framework for online model-free Reinforcement Learning on continuous
	control problems

	Copyright (c) 2016 SimionSoft. https://github.com/simionsoft

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#include "../app.h"
#include "../noise.h"

#include "robot-control.h"
#include "Robot.h"
#include "BulletPhysics.h"
#include "BulletBody.h"
#include "aux-rewards.h"
#include "Box.h"


RobotControl::RobotControl(ConfigNode* pConfigNode)
{
	METADATA("World", "Robot-control");

	m_target_X = addStateVariable("target-x", "m", -20.0, 20.0);
	m_target_Y = addStateVariable("target-y", "m", -20.0, 20.0);

	m_rob1_X = addStateVariable("robot1-x", "m", -20.0, 20.0);
	m_rob1_Y = addStateVariable("robot1-y", "m", -20.0, 20.0);
	m_theta = addStateVariable("robot1-theta", "rad", -3.1415, 3.1415, true);

	m_linear_vel = addActionVariable("robot1-v", "m/s", -2.0, 2.0);
	m_omega = addActionVariable("robot1-omega", "rad", -8.0, 8.0);

	MASS_ROBOT = 0.5f;
	MASS_GROUND = 0.f;
	MASS_TARGET = 0.1f;

	m_pBulletPhysics = new BulletPhysics();
	m_pBulletPhysics->initPhysics();
	m_pBulletPhysics->initPlayground();
	
	/// Creating target point, kinematic
	{
		KinematicObject* pTarget = new KinematicObject(MASS_TARGET
			, btVector3(BulletPhysics::TargetX, 0, BulletPhysics::TargetY)
			, new btConeShape(btScalar(0.5), btScalar(0.001)));
		pTarget->setAbsoluteStateVarIds("target-x", "target-y");
		m_pBulletPhysics->add(pTarget);
	}

	///creating a dynamic robot  
	{
		Robot* pRobot1 = new Robot(MASS_ROBOT
			, btVector3(BulletPhysics::r1origin_x, 0, BulletPhysics::r1origin_y)
			, new btSphereShape(0.5));
		pRobot1->setAbsoluteStateVarIds("robot1-x", "robot1-y", "robot1-theta");
		pRobot1->setActionIds("robot1-v", "robot1-omega");
		m_pBulletPhysics->add(pRobot1);
	}


	//the reward function
	m_pRewardFunction->addRewardComponent(new DistanceReward2D(getStateDescriptor(),"robot1-x","robot1-y","target-x","target-y"));
	m_pRewardFunction->initialize();
}

void RobotControl::reset(State *s)
{
	m_pBulletPhysics->reset(s);
}

void RobotControl::executeAction(State *s, const Action *a, double dt)
{
	btTransform trans;
	m_pBulletPhysics->updateBulletState(s, a, dt);

	//Update Robot1
	m_pBulletPhysics->stepSimulation((float)dt,20);

	//Update
	m_pBulletPhysics->updateState(s);
}

RobotControl::~RobotControl()
{
	delete m_pBulletPhysics;
}

