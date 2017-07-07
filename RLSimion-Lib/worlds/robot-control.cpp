#include "../stdafx.h"
#include "../app.h"
#include "../noise.h"

#include "robot-control.h"
#include "Robot.h"
#include "BulletPhysics.h"
#include "BulletBody.h"
#include "aux-rewards.h"
#include "Box.h"


CRobotControl::CRobotControl(CConfigNode* pConfigNode)
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
		pTarget->setAbsoluteStateVarIds(getStateDescriptor().getVarIndex("target-x")
			, getStateDescriptor().getVarIndex("target-y"), -1);
		m_pBulletPhysics->add(pTarget);
	}

	///creating a dynamic robot  
	{
		Robot* pRobot1 = new Robot(MASS_ROBOT
			, btVector3(BulletPhysics::r1origin_x, 0, BulletPhysics::r1origin_y)
			, new btSphereShape(0.5));
		pRobot1->setAbsoluteStateVarIds(getStateDescriptor().getVarIndex("robot1-x")
			, getStateDescriptor().getVarIndex("robot1-y")
			, getStateDescriptor().getVarIndex("robot1-theta"));
		pRobot1->setActionIds(getActionDescriptor().getVarIndex("robot1-v")
			, getActionDescriptor().getVarIndex("robot1-omega"));
		m_pBulletPhysics->add(pRobot1);
	}


	//the reward function
	m_pRewardFunction->addRewardComponent(new CDistanceReward2D(getStateDescriptor(),m_rob1_X,m_rob1_Y,m_target_X,m_target_Y));
	m_pRewardFunction->initialize();
}

void CRobotControl::reset(CState *s)
{
	m_pBulletPhysics->reset(s);
}

void CRobotControl::executeAction(CState *s, const CAction *a, double dt)
{
	btTransform trans;
	m_pBulletPhysics->updateBulletState(s, a, dt);

	//Update Robot1
	m_pBulletPhysics->stepSimulation((float)dt,20);

	//Update
	m_pBulletPhysics->updateState(s);
}

CRobotControl::~CRobotControl()
{
	delete m_pBulletPhysics;
}

