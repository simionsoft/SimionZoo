#include "pull-box-1.h"
#include "../../../3rd-party/bullet3-2.86/src/btBulletDynamicsCommon.h"
#include "../app.h"
#include "../noise.h"
#include "BulletBody.h"
#include "Robot.h"
#include "Box.h"
#include "BulletPhysics.h"
#include "Rope.h"
#include "aux-rewards.h"

double static getRand(double range)
{
	return (-range*0.5) + (range)*getRandomValue();
}

PullBox1::PullBox1(ConfigNode* pConfigNode)
{
	METADATA("World", "Pull-Box-1");
	m_target_X = addStateVariable("target-x", "m", -20.0, 20.0);
	m_target_Y = addStateVariable("target-y", "m", -20.0, 20.0);

	m_rob1_X = addStateVariable("robot1-x", "m", -20.0, 20.0);
	m_rob1_Y = addStateVariable("robot1-y", "m", -20.0, 20.0);

	m_box_X = addStateVariable("box-x", "m", -20.0, 20.0);
	m_box_Y = addStateVariable("box-y", "m", -20.0, 20.0);

	m_theta_r1 = addStateVariable("robot1-theta", "rad", -3.15, 3.15, true);
	m_boxTheta = addStateVariable("box-theta", "rad", -3.15, 3.15, true);
	m_D_BtX = addStateVariable("box-to-target-x", "m", -20.0, 20.0);
	m_D_BtY = addStateVariable("box-to-target-y", "m", -20.0, 20.0);

	m_D_Br1X = addStateVariable("robot1-to-box-x", "m", -6.0, 6.0);
	m_D_Br1Y = addStateVariable("robot1-to-box-y", "m", -6.0, 6.0);

	addActionVariable("robot1-v", "m/s", -2.0, 2.0);
	addActionVariable("robot1-omega", "rad/s", -8.0, 8.0);

	///Init Bullet
	m_pBulletPhysics = new BulletPhysics();

	m_pBulletPhysics->initSoftPhysics();
	m_pBulletPhysics->initPlayground();

	/// Creating target point, kinematic
	{
		KinematicObject* pTarget = new KinematicObject(BulletPhysics::MASS_TARGET
			, btVector3(BulletPhysics::TargetX, BulletPhysics::TargetZ, BulletPhysics::TargetY)
			, new btConeShape(btScalar(0.5), btScalar(0.001)));
		pTarget->setAbsoluteStateVarIds("target-x", "target-y");
		m_pBulletPhysics->add(pTarget);
	}

	///Creating dynamic box
	BulletBox* pBox = new BulletBox(BulletPhysics::MASS_BOX
		, btVector3(BulletPhysics::boxOrigin_x, BulletPhysics::boxOrigin_z, BulletPhysics::boxOrigin_y)
		, new btBoxShape(btVector3(btScalar(0.6), btScalar(0.6), btScalar(0.6))));
	pBox->setAbsoluteStateVarIds("box-x", "box-y", "box-theta");
	pBox->setRelativeStateVarIds("box-to-target-x", "box-to-target-y", "target-x", "target-y");
	m_pBulletPhysics->add(pBox);
	
	///creating  dynamic robot one
	Robot* pRobot1 = new Robot(BulletPhysics::MASS_ROBOT
		, btVector3(BulletPhysics::r1origin_x, BulletPhysics::r1origin_z, BulletPhysics::r1origin_y)
		, new btSphereShape(btScalar(0.5)));
	pRobot1->setAbsoluteStateVarIds("robot1-x", "robot1-y", "robot1-theta");
	pRobot1->setActionIds("robot1-v", "robot1-omega");
	pRobot1->setRelativeStateVarIds("robot1-to-box-x", "robot1-to-box-y", "box-x", "box-y");
	m_pBulletPhysics->add(pRobot1);

	/// creating an union with rope between robot and box
	m_pBulletPhysics->connectWithRope(pRobot1->getBody(), pBox->getBody());
	Rope* pRope = new Rope(this, m_pBulletPhysics->getSoftBodiesArray());
	m_pBulletPhysics->add(pRope);

	//the reward function
	m_pRewardFunction->addRewardComponent(new DistanceReward2D(getStateDescriptor(),"box-x", "box-y", "target-x", "target-y"));
	m_pRewardFunction->initialize();
}

void PullBox1::reset(State *s)
{
	m_pBulletPhysics->reset(s);
}

void PullBox1::executeAction(State *s, const Action *a, double dt)
{
	m_pBulletPhysics->updateBulletState(s, a, dt);

	//Execute simulation
	m_pBulletPhysics->simulate(dt, 20);

	//Update
	m_pBulletPhysics->updateState(s);
}

PullBox1::~PullBox1()
{
	delete m_pBulletPhysics;
}