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

	m_error = addStateVariable("error-z", "m", -60.0, 60.0);
	m_error_x = addStateVariable("errorX", "m", -60.0, 60.0);
	m_error_y = addStateVariable("errorY", "m", -60.0, 60.0);

	m_target_X = addStateVariable("target-x", "m", -60.0, 60.0);
	m_target_Y = addStateVariable("target-y", "m", -60.0, 60.0);
	m_target_Z = addStateVariable("target-z", "m", -60.0, 60.0);

	m_base_X = addStateVariable("base-x", "m", -60.0, 60.0);
	m_base_Y = addStateVariable("base-y", "m", -60.0, 60.0);
	m_base_Z = addStateVariable("base-z", "m", -60.0, 60.0);

	m_rotacion_base_X = addStateVariable("base-rot-x", "rad", -8.0, 8.0);
	m_rotacion_base_Y = addStateVariable("base-rot-y", "rad", -8.0, 8.0);
	m_rotacion_base_Z = addStateVariable("base-rot-z", "rad", -8.0, 8.0);

	m_angular_base_X = addStateVariable("base-angular-x", "rad/s", -25.0, 25.0);
	m_angular_base_Y = addStateVariable("base-angular-y", "rad/s", -25.0, 25.0);
	m_angular_base_Z = addStateVariable("base-angular-z", "rad/s", -25.0, 25.0);

	m_linear_base_X = addStateVariable("base-linear-x", "m/s", -25.0, 25.0);
	m_linear_base_Y = addStateVariable("base-linear-y", "m/s", -25.0, 25.0);
	m_linear_base_Z = addStateVariable("base-linear-z", "m/s", -25.0, 25.0);


	m_drone1_X = addStateVariable("drone1-x", "m", -60.0, 60.0);
	m_drone1_Y = addStateVariable("drone1-y", "m", -60.0, 60.0);
	m_drone1_Z = addStateVariable("drone1-z", "m", -60.0, 60.0);

	m_rotacion_drone1_X = addStateVariable("drone1-rot-x", "rad", -8.0, 8.0);
	m_rotacion_drone1_Y = addStateVariable("drone1-rot-y", "rad", -8.0, 8.0);
	m_rotacion_drone1_Z = addStateVariable("drone1-rot-z", "rad", -8.0, 8.0);

	m_angular_drone1_X = addStateVariable("drone1-angular-x", "rad/s", -25.0, 25.0);
	m_angular_drone1_Y = addStateVariable("drone1-angular-y", "rad/s", -25.0, 25.0);
	m_angular_drone1_Z = addStateVariable("drone1-angular-z", "rad/s", -25.0, 25.0);

	m_linear_drone1_X = addStateVariable("drone1-linear-x", "m/s", -25.0, 25.0);
	m_linear_drone1_Y = addStateVariable("drone1-linear-y", "m/s", -25.0, 25.0);
	m_linear_drone1_Z = addStateVariable("drone1-linear-z", "m/s", -25.0, 25.0);

	m_drone2_X = addStateVariable("drone2-x", "m", -60.0, 60.0);
	m_drone2_Y = addStateVariable("drone2-y", "m", -60.0, 60.0);
	m_drone2_Z = addStateVariable("drone2-z", "m", -60.0, 60.0);

	m_rotacion_drone2_X = addStateVariable("drone2-rot-x", "rad", -8.0, 8.0);
	m_rotacion_drone2_Y = addStateVariable("drone2-rot-y", "rad", -8.0, 8.0);
	m_rotacion_drone2_Z = addStateVariable("drone2-rot-z", "rad", -8.0, 8.0);

	m_angular_drone2_X = addStateVariable("drone2-angular-x", "rad/s", -25.0, 25.0);
	m_angular_drone2_Y = addStateVariable("drone2-angular-y", "rad/s", -25.0, 25.0);
	m_angular_drone2_Z = addStateVariable("drone2-angular-z", "rad/s", -25.0, 25.0);
	m_linear_drone2_X = addStateVariable("drone2-linear-x", "m/s", -25.0, 25.0);
	m_linear_drone2_Y = addStateVariable("drone2-linear-y", "m/s", -25.0, 25.0);
	m_linear_drone2_Z = addStateVariable("drone2-linear-z", "m/s", -25.0, 25.0);


	m_drone3_X = addStateVariable("drone3-x", "m", -60.0, 60.0);
	m_drone3_Y = addStateVariable("drone3-y", "m", -60.0, 60.0);
	m_drone3_Z = addStateVariable("drone3-z", "m", -60.0, 60.0);

	m_rotacion_drone3_X = addStateVariable("drone3-rot-x", "rad", -8.0, 8.0);
	m_rotacion_drone3_Y = addStateVariable("drone3-rot-y", "rad", -8.0, 8.0);
	m_rotacion_drone3_Z = addStateVariable("drone3-rot-z", "rad", -8.0, 8.0);

	m_angular_drone3_X = addStateVariable("drone3-angular-x", "rad/s", -25.0, 25.0);
	m_angular_drone3_Y = addStateVariable("drone3-angular-y", "rad/s", -25.0, 25.0);
	m_angular_drone3_Z = addStateVariable("drone3-angular-z", "rad/s", -25.0, 25.0);
	m_linear_drone3_X = addStateVariable("drone3-linear-x", "m/s", -25.0, 25.0);
	m_linear_drone3_Y = addStateVariable("drone3-linear-y", "m/s", -25.0, 25.0);
	m_linear_drone3_Z = addStateVariable("drone3-linear-z", "m/s", -25.0, 25.0);


	m_drone4_X = addStateVariable("drone4-x", "m", -60.0, 60.0);
	m_drone4_Y = addStateVariable("drone4-y", "m", -60.0, 60.0);
	m_drone4_Z = addStateVariable("drone4-z", "m", -60.0, 60.0);

	m_rotacion_drone4_X = addStateVariable("drone4-rot-x", "rad", -8.0, 8.0);
	m_rotacion_drone4_Y = addStateVariable("drone4-rot-y", "rad", -8.0, 8.0);
	m_rotacion_drone4_Z = addStateVariable("drone4-rot-z", "rad", -8.0, 8.0);

	m_angular_drone4_X = addStateVariable("drone4-angular-x", "rad/s", -25.0, 25.0);
	m_angular_drone4_Y = addStateVariable("drone4-angular-y", "rad/s", -25.0, 25.0);
	m_angular_drone4_Z = addStateVariable("drone4-angular-z", "rad/s", -25.0, 25.0);
	m_linear_drone4_X = addStateVariable("drone4-linear-x", "m/s", -25.0, 25.0);
	m_linear_drone4_Y = addStateVariable("drone4-linear-y", "m/s", -25.0, 25.0);
	m_linear_drone4_Z = addStateVariable("drone4-linear-z", "m/s", -25.0, 25.0);

	m_d_error = addStateVariable("d-error-z", "m/s", -2000, 2000);


	//289 menos,285mas,287 mas,288 mas,2885mas 725001430512 48888 425-725001430512 50000
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

	/// Creating target point, kinematic
	{
		KinematicObject* pTarget = new KinematicObject(MASS_TARGET
			, btVector3(BulletPhysics::TargetX, 0, BulletPhysics::TargetY)
			, new btConeShape(btScalar(0.5), btScalar(0.001)));
		pTarget->setAbsoluteStateVarIds("target-x", "target-y");
		m_pBulletPhysics->add(pTarget);
	}

	///creating a DRONE  
	{
		Drone6DOF* pDrone = new Drone6DOF(m_pBulletPhysics, btVector3(0., 0.05, 0.));
		pDrone->setActionIds("force1-1", "force1-2", "force1-3", "force1-4",
			"force2-1", "force2-2", "force2-3", "force2-4",
			"force3-1", "force3-2", "force2-3", "force3-4",
			"force4-1", "force4-2", "force4-3", "force4-4");
		pDrone->setAbsoluteStateVarIds("base-x", "base-y", "base-z", "base-rot-x", "base-rot-y", "base-rot-z", "base-angular-x", "base-angular-y", "base-angular-z", "base-linear-x", "base-linear-y", "base-linear-z",
			"drone1-x", "drone1-y", "drone1-z", "drone1-rot-x", "drone1-rot-y", "drone1-rot-z", "drone1-angular-x", "drone1-angular-y", "drone1-angular-z", "drone1-linear-x", "drone1-linear-y", "drone1-linear-z",
			"drone2-x", "drone2-y", "drone2-z", "drone2-rot-x", "drone2-rot-y", "drone2-rot-z", "drone2-angular-x", "drone2-angular-y", "drone2-angular-z", "drone2-linear-x", "drone2-linear-y", "drone2-linear-z",
			"drone3-x", "drone3-y", "drone3-z", "drone3-rot-x", "drone3-rot-y", "drone3-rot-z", "drone3-angular-x", "drone3-angular-y", "drone3-angular-z", "drone3-linear-x", "drone3-linear-y", "drone3-linear-z",
			"drone4-x", "drone4-y", "drone4-z", "drone4-rot-x", "drone4-rot-y", "drone4-rot-z", "drone4-angular-x", "drone4-angular-y", "drone4-angular-z", "drone4-linear-x", "drone4-linear-y", "drone4-linear-z");
		pDrone->setErrorStateVarId("error-z");
		//pDrone->setPIDActionId("force-PID");
		m_pBulletPhysics->addBody(pDrone);

	}

	FILE_PATH_PARAM filename = FILE_PATH_PARAM(pConfigNode, "Set-Point-File", "The setpoint file", "../config/world/drone6DOF/setpoint.txt");
	m_pSetpoint = new FileSetPoint(filename.get());

	//the reward function
	//m_pRewardFunction->addRewardComponent(new DistanceReward2D(getStateDescriptor(),"base-x","base-y","target-x","target-y"));
	m_pRewardFunction->addRewardComponent(new DistanceReward3D(getStateDescriptor(), "base-x", "base-y", "base-z", "base-rot-x", "base-rot-z", "base-linear-y", "target-x", "target-y", "error-z"));
	m_pRewardFunction->initialize();
}

void Drone6DOFControl::reset(State* s)
{
	double x;
	x = 0.0;
	s->set("base-y", x);

	if (SimionApp::get()->pExperiment->isEvaluationEpisode())
	{
		//fixed setting in evaluation episodes
		//s->set("target-z", (10.0));
		s->set(m_target_Z, m_pSetpoint->getPointSet(0.0));

	}
	else
	{
		//random setting in training episodes
		//aldatu 
		x = getRandomValue() * 5. + 5.;
		s->set("target-z", (0.0));

	}
	m_pBulletPhysics->reset(s);
}

void Drone6DOFControl::executeAction(State* s, const Action* a, double dt)
{
	if (SimionApp::get()->pExperiment->isEvaluationEpisode())
	{
		double setpoint_pitch = m_pSetpoint->getPointSet(SimionApp::get()->pWorld->getEpisodeSimTime());
		s->set(m_target_Z, setpoint_pitch);
	}
	else
	{
		double setpoint_pitch = m_pSetpoint->getPointSet(SimionApp::get()->pWorld->getEpisodeSimTime());
		//setpoint_pitch = setpoint_pitch / 2 + getRandomValue()*setpoint_pitch;
		s->set(m_target_Z, setpoint_pitch);
	}
	btTransform trans;
	//balioa t-1
	double error_z_previo = s->get("error-z");
	m_pBulletPhysics->updateBulletState(s, a, dt);

	//Update Drone
	m_pBulletPhysics->stepSimulation((float)dt, 20);

	//Update
	m_pBulletPhysics->updateState(s);

	//eguneratu
	double error_z = s->get("error-z");
	double d_error_z = (error_z - error_z_previo) / SimionApp::get()->pWorld->getDT();
	s->set("d-error-z", d_error_z);

}

Drone6DOFControl::~Drone6DOFControl()
{
	delete m_pBulletPhysics;
	delete m_pSetpoint;
}

