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
#include "../experiment.h"

#include "drone-6-dof-control.h"
#include "drone-6-dof.h"
#include "BulletPhysics.h"
#include "BulletBody.h"
#include "aux-rewards.h"
#include "Box.h"
#include "setpoint.h"


Drone6DOFControl::Drone6DOFControl(ConfigNode* pConfigNode)
{
	METADATA("World", "Drone-control");

	m_error_x = addStateVariable("error-x", "m", -60.0, 60.0);
	m_error_y = addStateVariable("error-y", "m", -60.0, 60.0);
	m_error_z = addStateVariable("error-z", "m", -60.0, 60.0);
	
	m_target_x = addStateVariable("target-x", "m", -60.0, 60.0);
	m_target_y = addStateVariable("target-y", "m", -60.0, 60.0);
	m_target_z = addStateVariable("target-z", "m", -60.0, 60.0);

	m_base_x = addStateVariable("base-x", "m", -60.0, 60.0);
	m_base_y = addStateVariable("base-y", "m", -60.0, 60.0);
	m_base_z = addStateVariable("base-z", "m", -60.0, 60.0);

	m_base_rot_x = addStateVariable("base-rot-x", "rad", -3.1416, 3.1416, true);
	m_base_rot_y = addStateVariable("base-rot-y", "rad", -3.1416, 3.1416, true);
	m_base_rot_z = addStateVariable("base-rot-z", "rad", -3.1416, 3.1416, true);

	m_base_angular_x = addStateVariable("base-angular-x", "rad/s", -25.0, 25.0);
	m_base_angular_y = addStateVariable("base-angular-y", "rad/s", -25.0, 25.0);
	m_base_angular_z = addStateVariable("base-angular-z", "rad/s", -25.0, 25.0);

	m_base_linear_x = addStateVariable("base-linear-x", "m/s", -25.0, 25.0);
	m_base_linear_y = addStateVariable("base-linear-y", "m/s", -25.0, 25.0);
	m_base_linear_z = addStateVariable("base-linear-z", "m/s", -25.0, 25.0);


	m_drone1_x = addStateVariable("drone1-x", "m", -60.0, 60.0);
	m_drone1_y = addStateVariable("drone1-y", "m", -60.0, 60.0);
	m_drone1_Z = addStateVariable("drone1-z", "m", -60.0, 60.0);

	m_drone1_rot_x = addStateVariable("drone1-rot-x", "rad", -3.1416, 3.1416, true);
	m_drone1_rot_y = addStateVariable("drone1-rot-y", "rad", -3.1416, 3.1416, true);
	m_drone1_rot_z = addStateVariable("drone1-rot-z", "rad", -3.1416, 3.1416, true);

	m_angular_drone1_X = addStateVariable("drone1-angular-x", "rad/s", -25.0, 25.0);
	m_angular_drone1_Y = addStateVariable("drone1-angular-y", "rad/s", -25.0, 25.0);
	m_angular_drone1_Z = addStateVariable("drone1-angular-z", "rad/s", -25.0, 25.0);

	m_drone1_linear_x = addStateVariable("drone1-linear-x", "m/s", -25.0, 25.0);
	m_drone1_linear_y = addStateVariable("drone1-linear-y", "m/s", -25.0, 25.0);
	m_drone1_linear_z = addStateVariable("drone1-linear-z", "m/s", -25.0, 25.0);

	m_drone2_x = addStateVariable("drone2-x", "m", -60.0, 60.0);
	m_drone2_y = addStateVariable("drone2-y", "m", -60.0, 60.0);
	m_drone2_z = addStateVariable("drone2-z", "m", -60.0, 60.0);

	m_drone2_rot_x = addStateVariable("drone2-rot-x", "rad", -3.1416, 3.1416, true);
	m_drone2_rot_y = addStateVariable("drone2-rot-y", "rad", -3.1416, 3.1416, true);
	m_drone2_rot_z = addStateVariable("drone2-rot-z", "rad", -3.1416, 3.1416, true);

	m_drone2_angular_x = addStateVariable("drone2-angular-x", "rad/s", -25.0, 25.0);
	m_drone2_angular_y = addStateVariable("drone2-angular-y", "rad/s", -25.0, 25.0);
	m_drone2_angular_z = addStateVariable("drone2-angular-z", "rad/s", -25.0, 25.0);

	m_drone2_linear_x = addStateVariable("drone2-linear-x", "m/s", -25.0, 25.0);
	m_drone2_linear_y = addStateVariable("drone2-linear-y", "m/s", -25.0, 25.0);
	m_drone2_linear_z = addStateVariable("drone2-linear-z", "m/s", -25.0, 25.0);

	m_drone3_x = addStateVariable("drone3-x", "m", -60.0, 60.0);
	m_drone3_y = addStateVariable("drone3-y", "m", -60.0, 60.0);
	m_drone3_z = addStateVariable("drone3-z", "m", -60.0, 60.0);

	m_drone3_rot_x = addStateVariable("drone3-rot-x", "rad", -3.1416, 3.1416, true);
	m_drone3_rot_y = addStateVariable("drone3-rot-y", "rad", -3.1416, 3.1416, true);
	m_drone3_rot_z = addStateVariable("drone3-rot-z", "rad", -3.1416, 3.1416, true);

	m_drone3_angular_x = addStateVariable("drone3-angular-x", "rad/s", -25.0, 25.0);
	m_drone3_angular_y = addStateVariable("drone3-angular-y", "rad/s", -25.0, 25.0);
	m_drone3_angular_z = addStateVariable("drone3-angular-z", "rad/s", -25.0, 25.0);

	m_drone3_linear_x = addStateVariable("drone3-linear-x", "m/s", -25.0, 25.0);
	m_drone3_linear_y = addStateVariable("drone3-linear-y", "m/s", -25.0, 25.0);
	m_drone3_linear_z = addStateVariable("drone3-linear-z", "m/s", -25.0, 25.0);

	m_drone4_x = addStateVariable("drone4-x", "m", -60.0, 60.0);
	m_drone4_y = addStateVariable("drone4-y", "m", -60.0, 60.0);
	m_drone4_z = addStateVariable("drone4-z", "m", -60.0, 60.0);

	m_drone4_rot_x = addStateVariable("drone4-rot-x", "rad", -3.1416, 3.1416, true);
	m_drone4_rot_y = addStateVariable("drone4-rot-y", "rad", -3.1416, 3.1416, true);
	m_drone4_rot_z = addStateVariable("drone4-rot-z", "rad", -3.1416, 3.1416, true);

	m_drone4_angular_x = addStateVariable("drone4-angular-x", "rad/s", -25.0, 25.0);
	m_drone4_angular_y = addStateVariable("drone4-angular-y", "rad/s", -25.0, 25.0);
	m_drone4_angular_z = addStateVariable("drone4-angular-z", "rad/s", -25.0, 25.0);

	m_drone4_linear_x = addStateVariable("drone4-linear-x", "m/s", -25.0, 25.0);
	m_drone4_linear_y = addStateVariable("drone4-linear-y", "m/s", -25.0, 25.0);
	m_drone4_linear_z = addStateVariable("drone4-linear-z", "m/s", -25.0, 25.0);

	m_d_error_x = addStateVariable("d-error-x", "m/s", -2000, 2000);
	m_d_error_y = addStateVariable("d-error-y", "m/s", -2000, 2000);
	m_d_error_z = addStateVariable("d-error-z", "m/s", -2000, 2000);

	addActionVariable("force-PID", "N", 0.0, 50.0);

	addActionVariable("force1-1", "N", 0.0, 50.0);
	addActionVariable("force1-2", "N", 0.0, 50.0);
	addActionVariable("force1-3", "N", 0.0, 50.0);
	addActionVariable("force1-4", "N", 0.0, 50.0);

	addActionVariable("force2-1", "N", 0.0, 50.0);
	addActionVariable("force2-2", "N", 0.0, 50.0);
	addActionVariable("force2-3", "N", 0.0, 50.0);
	addActionVariable("force2-4", "N", 0.0, 50.0);

	addActionVariable("force3-1", "N", 0.0, 50.0);
	addActionVariable("force3-2", "N", 0.0, 50.0);
	addActionVariable("force3-3", "N", 0.0, 50.0);
	addActionVariable("force3-4", "N", 0.0, 50.0);

	addActionVariable("force4-1", "N", 0.0, 50.0);
	addActionVariable("force4-2", "N", 0.0, 50.0);
	addActionVariable("force4-3", "N", 0.0, 50.0);
	addActionVariable("force4-4", "N", 0.0, 50.0);

	MASS_ROBOT = 0.5f;
	MASS_GROUND = 0.f;
	MASS_TARGET = 0.1f;

	m_pBulletPhysics = new BulletPhysics();
	m_pBulletPhysics->initPhysics();
	m_pBulletPhysics->initPlayground();

	///// Creating target point, kinematic
	//{
	//	KinematicObject* pTarget = new KinematicObject(MASS_TARGET
	//		, btVector3(BulletPhysics::TargetX, 0, BulletPhysics::TargetY)
	//		, new btConeShape(btScalar(0.5), btScalar(0.001)));
	//	pTarget->setAbsoluteStateVarIds("target-x", "target-y");
	//	m_pBulletPhysics->add(pTarget);
	//}

	///creating a DRONE  
	{
		Drone6DOF* pDrone = new Drone6DOF(m_pBulletPhysics);
		pDrone->setActionIds("force1-1", "force1-2", "force1-3", "force1-4",
			"force2-1", "force2-2", "force2-3", "force2-4",
			"force3-1", "force3-2", "force2-3", "force3-4",
			"force4-1", "force4-2", "force4-3", "force4-4");
		pDrone->setAbsoluteStateVarIds("base-x", "base-y", "base-z",
			"base-rot-x", "base-rot-y", "base-rot-z",
			"base-angular-x", "base-angular-y", "base-angular-z",
			"base-linear-x", "base-linear-y", "base-linear-z");
		
		m_pBulletPhysics->addBody(pDrone);

	}

	FILE_PATH_PARAM setpointFileX = FILE_PATH_PARAM(pConfigNode, "Set-Point-File-X", "The setpoint file for the x coordinate", "../config/world/drone6DOF/setpoint-x.txt");
	FILE_PATH_PARAM setpointFileY = FILE_PATH_PARAM(pConfigNode, "Set-Point-File-Y", "The setpoint file for the y coordinate", "../config/world/drone6DOF/setpoint-y.txt");
	FILE_PATH_PARAM setpointFileZ = FILE_PATH_PARAM(pConfigNode, "Set-Point-File-Z", "The setpoint file for the z coordinate", "../config/world/drone6DOF/setpoint-z.txt");
	m_pSetpointX = new FileSetPoint(setpointFileX.get());
	m_pSetpointY = new FileSetPoint(setpointFileY.get());
	m_pSetpointZ = new FileSetPoint(setpointFileZ.get());

	//the reward function
	m_pRewardFunction->addRewardComponent(new DistanceReward3D(getStateDescriptor(), "error-x", "error-y", "error-z", 1.0));
	m_pRewardFunction->initialize();
}

void Drone6DOFControl::reset(State* s)
{
	double start_x, start_y, start_z;
	double target_x, target_y, target_z;
	double randomRange;
	const double minRandomRange = 1.0;
	const double maxRandomRange = 10.0;

	if (SimionApp::get()->pExperiment->isEvaluationEpisode())
	{
		//start from the setpoint start point
		start_x = m_pSetpointX->getSetPoint(0.0);
		start_y = m_pSetpointY->getSetPoint(0.0);
		start_z = m_pSetpointZ->getSetPoint(0.0);
		target_x = m_pSetpointX->getSetPoint(0.0);
		target_y = m_pSetpointY->getSetPoint(0.0);
		target_z = m_pSetpointZ->getSetPoint(0.0);
	}
	else
	{
		randomRange = minRandomRange + (maxRandomRange - minRandomRange) 
			* SimionApp::get()->pExperiment->getExperimentProgress();

		//random settings in training episodes
		start_x = -(randomRange * 0.5) + getRandomValue() * randomRange;
		start_y = getRandomValue() * randomRange * 2.0;
		start_z = -(randomRange * 0.5) + getRandomValue() * randomRange;
		target_x = -(randomRange * 0.5) + getRandomValue() * randomRange;
		target_y = getRandomValue() * randomRange * 2.0;
		target_z = -(randomRange * 0.5) + getRandomValue() * randomRange ;
	}
	s->set("base-x", start_x);
	s->set("base-y", start_y);
	s->set("base-z", start_z);
	s->set("target-x", target_x);
	s->set("target-y", target_y);
	s->set("target-z", target_z);
	s->set("error-x", target_x - start_x);
	s->set("error-y", target_y - start_y);
	s->set("error-z", target_z - start_z);

	m_pBulletPhysics->reset(s);
}

void Drone6DOFControl::executeAction(State* s, const Action* a, double dt)
{
	if (SimionApp::get()->pExperiment->isEvaluationEpisode())
	{
		double setpointX = m_pSetpointX->getSetPoint(SimionApp::get()->pWorld->getEpisodeSimTime());
		double setpointY = m_pSetpointY->getSetPoint(SimionApp::get()->pWorld->getEpisodeSimTime());
		double setpointZ = m_pSetpointZ->getSetPoint(SimionApp::get()->pWorld->getEpisodeSimTime());
		s->set(m_target_x, setpointX);
		s->set(m_target_y, setpointY);
		s->set(m_target_z, setpointZ);
	}

	btTransform trans;
	
	double lastXError = s->get("error-x");
	double lastYError = s->get("error-y");
	double lastZError = s->get("error-z");
	m_pBulletPhysics->updateBulletState(s, a, dt);

	//Update Drone
	m_pBulletPhysics->stepSimulation((float)dt, 20);

	//Update
	m_pBulletPhysics->updateState(s);

	//calculate error derivatives
	double error_x = s->get("error-x");
	double error_y = s->get("error-y");
	double error_z = s->get("error-z");

	double d_error_x = (error_x - lastXError) / SimionApp::get()->pWorld->getDT();
	s->set("d-error-x", d_error_x);
	double d_error_y = (error_y - lastYError) / SimionApp::get()->pWorld->getDT();
	s->set("d-error-y", d_error_y);
	double d_error_z = (error_z - lastZError) / SimionApp::get()->pWorld->getDT();
	s->set("d-error-z", d_error_z);

}

Drone6DOFControl::~Drone6DOFControl()
{
	delete m_pBulletPhysics;
	delete m_pSetpointX;
}

