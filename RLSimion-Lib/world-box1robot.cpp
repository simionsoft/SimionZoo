#include "stdafx.h"
#include "world-box1robot.h"
#include "app.h"
#include "noise.h"
#include "BulletPhysics.h"
#include "BulletDisplay.h"
#include "BulletBody.h"
#include "Box.h"
#include "Robot.h"
#include "world-aux-rewards.h"
#pragma comment(lib,"opengl32.lib")

double static getRand(double range)
{
	return (-range*0.5) + (range)*getRandomValue();
}

#define TargetX 10.0
#define TargetY 3.0

#define ground_x 0.0
#define ground_y -1.0
#define ground_z 0.0 

#define robotOrigin_x 0.0
#define robotOrigin_y 0.0

#define boxOrigin_x 3.0
#define boxOrigin_y 0.0

#define theta_o 0.0

CMoveBoxOneRobot::CMoveBoxOneRobot(CConfigNode* pConfigNode)
{
	METADATA("World", "MoveBoxOneRobot");

	m_target_X = addStateVariable("targetX", "m", -20.0, 20.0);
	m_target_Y = addStateVariable("targetY", "m", -20.0, 20.0);
	m_rob1_X = addStateVariable("rx1", "m", -10.0, 10.0);
	m_rob1_Y = addStateVariable("ry1", "m", -10.0, 10.0);
	m_box_X = addStateVariable("bx", "m", -10.0, 10.0);
	m_box_Y = addStateVariable("by", "m", -10.0, 10.0);

	m_D_BrX = addStateVariable("dBrX", "m", -10.0, 10.0);
	m_D_BrY = addStateVariable("dBrY", "m", -10.0, 10.0);
	m_D_BtX = addStateVariable("dBtX", "m", -10.0, 10.0);
	m_D_BtY = addStateVariable("dBtY", "m", -10.0, 10.0);
	m_theta = addStateVariable("theta", "rad", -3.15, 3.15, true);
	m_boxTheta = addStateVariable("boxTheta", "rad", -3.15, 3.15, true);

	m_linear_vel = addActionVariable("v", "m/s", -2.0, 2.0);
	m_omega = addActionVariable("omega", "rad/s", -8.0, 8.0);

	MASS_ROBOT = 1.5f;
	MASS_BOX = 1.f;
	MASS_GROUND = 0.f;
	MASS_TARGET = 0.1f;

	rBoxPhysics = new BulletPhysics();
	if (!CSimionApp::get()->isExecutedRemotely())
		rBoxGraphics = new BulletGraphic();

	rBoxPhysics->initPhysics();
	if (!CSimionApp::get()->isExecutedRemotely())
		rBoxGraphics->setDebugger(rBoxPhysics->getDynamicsWorld());

	///Creating static object, ground
	{
		m_Ground = new BulletBody(MASS_GROUND, btVector3(ground_x, ground_y, ground_z), new btBoxShape(btVector3(btScalar(20.), btScalar(1.), btScalar(20.))), false);
		rBoxPhysics->getCollisionShape().push_back(m_Ground->getShape());
		rBoxPhysics->getDynamicsWorld()->addRigidBody(m_Ground->getBody());
	}
	///Creating static object, walls
	{
		m_pWall1 = new BulletBody(MASS_GROUND, btVector3(12.0, 1.0, 0.0), new btBoxShape(btVector3(1.0, 2.0, 13.)), false);
		rBoxPhysics->getCollisionShape().push_back(m_pWall1->getShape());
		rBoxPhysics->getDynamicsWorld()->addRigidBody(m_pWall1->getBody());
		m_pWall2 = new BulletBody(MASS_GROUND, btVector3(-12.0, 1.0, 0.0), new btBoxShape(btVector3(1.0, 2.0, 13.)), false);
		rBoxPhysics->getCollisionShape().push_back(m_pWall2->getShape());
		rBoxPhysics->getDynamicsWorld()->addRigidBody(m_pWall2->getBody());
		m_pWall3 = new BulletBody(MASS_GROUND, btVector3(1.0, 1.0, 12.0), new btBoxShape(btVector3(12.0, 2.0, 1.0)), false);
		rBoxPhysics->getCollisionShape().push_back(m_pWall3->getShape());
		rBoxPhysics->getDynamicsWorld()->addRigidBody(m_pWall3->getBody());
		m_pWall4 = new BulletBody(MASS_GROUND, btVector3(1.0, 1.0, -12.0), new btBoxShape(btVector3(12.0, 2.0, 1.0)), false);
		rBoxPhysics->getCollisionShape().push_back(m_pWall4->getShape());
		rBoxPhysics->getDynamicsWorld()->addRigidBody(m_pWall4->getBody());
	}

	/// Creating target point, static
	{
		m_Target = new BulletBody(MASS_TARGET, btVector3(TargetX, 0.001, TargetY), new btConeShape(0.5, 0.001), false);
		m_Target->getBody()->setCollisionFlags(m_Target->getBody()->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		rBoxPhysics->getCollisionShape().push_back(m_Target->getShape());
		rBoxPhysics->getDynamicsWorld()->addRigidBody(m_Target->getBody());
	}

	///Creating dynamic box
	{
		m_Box = new Box(MASS_BOX, btVector3(boxOrigin_x, 0, boxOrigin_y), new btBoxShape(btVector3(0.6, 0.6, 0.6)), true);
		rBoxPhysics->getCollisionShape().push_back(m_Box->getShape());
		rBoxPhysics->getDynamicsWorld()->addRigidBody(m_Box->getBody());
	}

	///creating a dynamic robot  
	{
		m_Robot = new Robot(MASS_ROBOT, btVector3(robotOrigin_x, 0, robotOrigin_y), new btSphereShape(0.5), true);
		rBoxPhysics->getCollisionShape().push_back(m_Robot->getShape());
		rBoxPhysics->getDynamicsWorld()->addRigidBody(m_Robot->getBody());
	}

	///Graphic init
	if (!CSimionApp::get()->isExecutedRemotely())
		rBoxGraphics->generateGraphics(rBoxPhysics->getDynamicsWorld());

	//the reward function
	m_pRewardFunction->addRewardComponent(new CBoxTargetReward(m_box_X,m_box_Y,m_target_X,m_target_Y));
	m_pRewardFunction->initialize();
}


void CMoveBoxOneRobot::reset(CState *s)
{
	if (1)//CSimionApp::get()->pExperiment->isEvaluationEpisode())
	{
		m_Box->reset(s, boxOrigin_x, boxOrigin_y, m_box_X, m_box_Y);
		m_Robot->reset(s, robotOrigin_x, robotOrigin_y, m_rob1_X, m_rob1_Y);

		///set initial values to state variables
		s->set(m_theta, theta_o);
		s->set(m_boxTheta, 0.0);
	}
	else
	{
		double boxOrX = boxOrigin_x + getRand(2.0);
		double boxOrY = boxOrigin_y + getRand(2.0);
		double robOrX = robotOrigin_x + getRand(2.0);
		double robOrY = robotOrigin_y + getRand(2.0);

		m_Box->reset(s, boxOrX, boxOrY, m_box_X, m_box_Y);
		m_Robot->reset(s, robOrX, robOrY, m_rob1_X, m_rob1_Y);

		s->set(m_theta, theta_o + getRand(1.0));
		s->set(m_boxTheta, 0.0);
	}

	//set relative coordinates
	s->set(m_D_BrX, fabs(s->get(m_box_X) - s->get(m_rob1_X)));
	s->set(m_D_BrY, fabs(s->get(m_box_Y) - s->get(m_rob1_Y)));
	s->set(m_D_BtX, fabs(s->get(m_box_X) - TargetX));
	s->set(m_D_BtY, fabs(s->get(m_box_Y) - TargetY));

	//target
	s->set(m_target_X, TargetX);
	s->set(m_target_Y, TargetY);
}

void CMoveBoxOneRobot::executeAction(CState *s, const CAction *a, double dt)
{
	double theta;
	theta = m_Robot->updateRobotMovement(a, s, "omega", "v", m_theta, dt);
	
	//Execute simulation
	rBoxPhysics->getDynamicsWorld()->stepSimulation(dt, 20);
	if (!CSimionApp::get()->isExecutedRemotely())
		rBoxGraphics->updateCamera();

	//Update

	btTransform box_trans = m_Box->setAbsoluteVariables(s, m_box_X, m_box_Y);
	m_Robot->setAbsoluteVariables(s, m_rob1_X, m_rob1_Y);
		
	m_Robot->setRelativeVariables(s, m_D_BrX, m_D_BrY, box_trans.getOrigin().getX(),box_trans.getOrigin().getZ());
	m_Box->setRelativeVariables(s, m_D_BtX, m_D_BtY,TargetX, TargetY);

	s->set(m_theta, theta);
	btTransform boxTransform;
	m_Box->getBody()->getMotionState()->getWorldTransform(boxTransform);
	btScalar yaw, pitch, roll;
	boxTransform.getBasis().getEulerYPR(yaw,pitch,roll);
	if (pitch < SIMD_2_PI) pitch += SIMD_2_PI;
	else if (pitch > SIMD_2_PI) pitch -= SIMD_2_PI;
	s->set(m_boxTheta, (double)yaw);

	//draw
	btVector3 printPosition = btVector3(TargetX, 5, TargetY);
	if (CSimionApp::get()->pExperiment->isEvaluationEpisode())
	{
		if (!CSimionApp::get()->isExecutedRemotely())
		{
			rBoxGraphics->drawText3D("Evaluation episode", printPosition);
			rBoxGraphics->drawDynamicWorld(rBoxPhysics->getDynamicsWorld());
		}
	}
	else
	{
		if (!CSimionApp::get()->isExecutedRemotely())
			rBoxGraphics->drawText3D("Training episode", printPosition);
	}
	if (!CSimionApp::get()->isExecutedRemotely())
	{
		//rBoxGraphics->drawDynamicWorld(rBoxPhysics->getDynamicsWorld());
	}
}

CMoveBoxOneRobot::~CMoveBoxOneRobot()
{
	delete m_Ground;
	delete m_Robot;
	delete m_Box;
	delete m_Target;
	delete m_pWall1;
	delete m_pWall2;
	delete m_pWall3;
	delete m_pWall4;
}