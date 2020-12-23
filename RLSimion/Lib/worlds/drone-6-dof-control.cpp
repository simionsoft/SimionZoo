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

#include "drone-6-dof-control.h"
#include "drone-6-dof.h"
#include "BulletPhysics.h"
#include "BulletBody.h"
#include "aux-rewards.h"
#include "../experiment.h"
#include "Box.h"


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

	m_rotacion_base_X = addStateVariable("base-rot-x", "rad", -3.1415, 3.1415, true);
	m_rotacion_base_Y = addStateVariable("base-rot-y", "rad", -3.1415, 3.1415, true);
	m_rotacion_base_Z = addStateVariable("base-rot-z", "rad", -3.1415, 3.1415, true);

	m_angular_base_X = addStateVariable("base-angular-x", "rad/s", -8.0, 8.0);
	m_angular_base_Y = addStateVariable("base-angular-y", "rad/s", -8.0, 8.0);
	m_angular_base_Z = addStateVariable("base-angular-z", "rad/s", -8.0, 8.0);

	m_linear_base_X = addStateVariable("base-linear-x", "m/s", -15.0, 15.0);
	m_linear_base_Y = addStateVariable("base-linear-y", "m/s", -15.0, 15.0);
	m_linear_base_Z = addStateVariable("base-linear-z", "m/s", -15.0, 15.0);

	m_drone1_X = addStateVariable("drone1-x", "m", -60.0, 60.0);
	m_drone1_Y = addStateVariable("drone1-y", "m", -60.0, 60.0);
	m_drone1_Z = addStateVariable("drone1-z", "m", -60.0, 60.0);

	m_rotacion_drone1_X = addStateVariable("drone1-rot-x", "rad", -3.1415, 3.1415, true);
	m_rotacion_drone1_Y = addStateVariable("drone1-rot-y", "rad", -3.1415, 3.1415, true);
	m_rotacion_drone1_Z = addStateVariable("drone1-rot-z", "rad", -3.1415, 3.1415, true);

	m_angular_drone1_X = addStateVariable("drone1-angular-x", "rad/s", -8.0, 8.0);
	m_angular_drone1_Y = addStateVariable("drone1-angular-y", "rad/s", -8.0, 8.0);
	m_angular_drone1_Z = addStateVariable("drone1-angular-z", "rad/s", -8.0, 8.0);

	m_linear_drone1_X = addStateVariable("drone1-linear-x", "m/s", -15.0, 15.0);
	m_linear_drone1_Y = addStateVariable("drone1-linear-y", "m/s", -15.0, 15.0);
	m_linear_drone1_Z = addStateVariable("drone1-linear-z", "m/s", -15.0, 15.0);

	m_drone2_X = addStateVariable("drone2-x", "m", -60.0, 60.0);
	m_drone2_Y = addStateVariable("drone2-y", "m", -60.0, 60.0);
	m_drone2_Z = addStateVariable("drone2-z", "m", -60.0, 60.0);

	m_rotacion_drone2_X = addStateVariable("drone2-rot-x", "rad", -3.1415, 3.1415, true);
	m_rotacion_drone2_Y = addStateVariable("drone2-rot-y", "rad", -3.1415, 3.1415, true);
	m_rotacion_drone2_Z = addStateVariable("drone2-rot-z", "rad", -3.1415, 3.1415, true);

	m_angular_drone2_X = addStateVariable("drone2-angular-x", "rad/s", -8.0, 8.0);
	m_angular_drone2_Y = addStateVariable("drone2-angular-y", "rad/s", -8.0, 8.0);
	m_angular_drone2_Z = addStateVariable("drone2-angular-z", "rad/s", -8.0, 8.0);

	m_linear_drone2_X = addStateVariable("drone2-linear-x", "m/s", -15.0, 15.0);
	m_linear_drone2_Y = addStateVariable("drone2-linear-y", "m/s", -15.0, 15.0);
	m_linear_drone2_Z = addStateVariable("drone2-linear-z", "m/s", -15.0, 15.0);

	m_drone3_X = addStateVariable("drone3-x", "m", -60.0, 60.0);
	m_drone3_Y = addStateVariable("drone3-y", "m", -60.0, 60.0);
	m_drone3_Z = addStateVariable("drone3-z", "m", -60.0, 60.0);

	m_rotacion_drone3_X = addStateVariable("drone3-rot-x", "rad", -3.1415, 3.1415, true);
	m_rotacion_drone3_Y = addStateVariable("drone3-rot-y", "rad", -3.1415, 3.1415, true);
	m_rotacion_drone3_Z = addStateVariable("drone3-rot-z", "rad", -3.1415, 3.1415, true);

	m_angular_drone3_X = addStateVariable("drone3-angular-x", "rad/s", -8.0, 8.0);
	m_angular_drone3_Y = addStateVariable("drone3-angular-y", "rad/s", -8.0, 8.0);
	m_angular_drone3_Z = addStateVariable("drone3-angular-z", "rad/s", -8.0, 8.0);
	
	m_linear_drone3_X = addStateVariable("drone3-linear-x", "m/s", -15.0, 15.0);
	m_linear_drone3_Y = addStateVariable("drone3-linear-y", "m/s", -15.0, 15.0);
	m_linear_drone3_Z = addStateVariable("drone3-linear-z", "m/s", -15.0, 15.0);


	m_drone4_X = addStateVariable("drone4-x", "m", -60.0, 60.0);
	m_drone4_Y = addStateVariable("drone4-y", "m", -60.0, 60.0);
	m_drone4_Z = addStateVariable("drone4-z", "m", -60.0, 60.0);

	m_rotacion_drone4_X = addStateVariable("drone4-rot-x", "rad", -3.1415, 3.1415, true);
	m_rotacion_drone4_Y = addStateVariable("drone4-rot-y", "rad", -3.1415, 3.1415, true);
	m_rotacion_drone4_Z = addStateVariable("drone4-rot-z", "rad", -3.1415, 3.1415, true);

	m_angular_drone4_X = addStateVariable("drone4-angular-x", "rad/s", -8.0, 8.0);
	m_angular_drone4_Y = addStateVariable("drone4-angular-y", "rad/s", -8.0, 8.0);
	m_angular_drone4_Z = addStateVariable("drone4-angular-z", "rad/s", -8.0, 8.0);

	m_linear_drone4_X = addStateVariable("drone4-linear-x", "m/s", -15.0, 15.0);
	m_linear_drone4_Y = addStateVariable("drone4-linear-y", "m/s", -15.0, 15.0);
	m_linear_drone4_Z = addStateVariable("drone4-linear-z", "m/s", -15.0, 15.0);

	m_d_error = addStateVariable("d-error-z", "m/s", -2000, 2000);
	

	//289 menos,285mas,287 mas,288 mas,2885mas 725001430512 48888 425-725001430512 50000
	addActionVariable("fuerza-PID", "N", 0.0,100.0);

	addActionVariable("fuerza1-1", "N", 0.0, 100.0);
	addActionVariable("fuerza1-2", "N", 0.0, 100.0);
	addActionVariable("fuerza1-3", "N", 0.0, 100.0);
	addActionVariable("fuerza1-4", "N", 0.0, 100.0);

	addActionVariable("fuerza2-1", "N", 0.0, 100.0);
	addActionVariable("fuerza2-2", "N", 0.0, 100.0);
	addActionVariable("fuerza2-3", "N", 0.0, 100.0);
	addActionVariable("fuerza2-4", "N", 0.0, 100.0);

	addActionVariable("fuerza3-1", "N", 0.0, 100.0);
	addActionVariable("fuerza3-2", "N", 0.0, 100.0);
	addActionVariable("fuerza3-3", "N", 0.0, 100.0);
	addActionVariable("fuerza3-4", "N", 0.0, 100.0);

	addActionVariable("fuerza4-1", "N", 0.0, 100.0);
	addActionVariable("fuerza4-2", "N", 0.0, 100.0);
	addActionVariable("fuerza4-3", "N", 0.0, 100.0);
	addActionVariable("fuerza4-4", "N", 0.0, 100.0);

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
		pDrone->setActionIds("fuerza1-1", "fuerza1-2", "fuerza1-3", "fuerza1-4",
			"fuerza2-1", "fuerza2-2", "fuerza2-3", "fuerza2-4",
			"fuerza3-1", "fuerza3-2", "fuerza2-3", "fuerza3-4",
			"fuerza4-1", "fuerza4-2", "fuerza4-3", "fuerza4-4");
		pDrone->setAbsoluteStateVarIds("base-x", "base-y", "base-z", "base-rot-x", "base-rot-y", "base-rot-z","base-angular-x","base-angular-y","base-angular-z", "base-linear-x","base-linear-y","base-linear-z",
			"drone1-x", "drone1-y", "drone1-z", "drone1-rot-x", "drone1-rot-y", "drone1-rot-z", "drone1-angular-x","drone1-angular-y","drone1-angular-z", "drone1-linear-x","drone1-linear-y","drone1-linear-z",
			"drone2-x", "drone2-y", "drone2-z", "drone2-rot-x", "drone2-rot-y", "drone2-rot-z", "drone2-angular-x","drone2-angular-y","drone2-angular-z", "drone2-linear-x","drone2-linear-y","drone2-linear-z",
			"drone3-x", "drone3-y", "drone3-z", "drone3-rot-x", "drone3-rot-y", "drone3-rot-z", "drone3-angular-x","drone3-angular-y","drone3-angular-z", "drone3-linear-x","drone3-linear-y","drone3-linear-z",
			"drone4-x", "drone4-y", "drone4-z", "drone4-rot-x", "drone4-rot-y", "drone4-rot-z", "drone4-angular-x","drone4-angular-y","drone4-angular-z", "drone4-linear-x","drone4-linear-y","drone4-linear-z");
		pDrone->setErrorStateVarId("error-z");
		//pDrone->setPIDActionId("fuerza-PID");
		m_pBulletPhysics->addBody(pDrone);
		
	}


	//the reward function
	//m_pRewardFunction->addRewardComponent(new DistanceReward2D(getStateDescriptor(),"base-x","base-y","target-x","target-y"));
    m_pRewardFunction->addRewardComponent(new DistanceReward3D(getStateDescriptor(), "base-x", "base-y", "base-z", "base-rot-x","base-rot-z","base-linear-y","target-x","target-y","error-z"));
	m_pRewardFunction->initialize();
}

void Drone6DOFControl::reset(State *s)
{
	double x;
	//if (SimionApp::get()->pExperiment->isEvaluationEpisode())
	{
		//fixed setting in evaluation episodes
		x = 0.0;
		s->set("base-y", x);
	}
	/*else
	{
		//random setting in training episodes
		//aldatu 
		x = getRandomValue()*59.95+0.05;    //[0.05, 60.00]
		s->set("base-y", x); 
	}*/
	m_pBulletPhysics->reset(s);
}

void Drone6DOFControl::executeAction(State *s, const Action *a, double dt)
{
	btTransform trans;

	//Some values need to be calculated here: z-error, linear speed
	double prev_error_z = s->get("error-z");
	
	//double prev_base_x = s->get("base-x");
	//double prev_base_y = s->get("base-y");
	//double prev_base_z = s->get("base-z");

	//double prev_drone1_x = s->get("drone1-x");
	//double prev_drone1_y = s->get("drone1-y");
	//double prev_drone1_z = s->get("drone1-z");

	//double prev_drone2_x = s->get("drone2-x");
	//double prev_drone2_y = s->get("drone2-y");
	//double prev_drone2_z = s->get("drone2-z");

	//double prev_drone3_x = s->get("drone3-x");
	//double prev_drone3_y = s->get("drone3-y");
	//double prev_drone3_z = s->get("drone3-z");

	//double prev_drone4_x = s->get("drone4-x");
	//double prev_drone4_y = s->get("drone4-y");
	//double prev_drone4_z = s->get("drone4-z");

	
	m_pBulletPhysics->updateBulletState(s, a, dt);

	//Update Drone
	m_pBulletPhysics->stepSimulation((float)dt,20);

	//Update
	m_pBulletPhysics->updateState(s);

	//Set state values calculated by hand
	double dtInv = 1.0 / SimionApp::get()->pWorld->getDT();

	s->set("d-error-z", (s->get("error-z") - prev_error_z) * dtInv);

	//s->set("base-linear-x", (s->get("base-x") - prev_base_x) * dtInv);
	//s->set("base-linear-y", (s->get("base-y") - prev_base_y) * dtInv);
	//s->set("base-linear-z", (s->get("base-z") - prev_base_z) * dtInv);

	//s->set("drone1-linear-x", (s->get("drone1-x") - prev_drone1_x) * dtInv);
	//s->set("drone1-linear-y", (s->get("drone1-y") - prev_drone1_y) * dtInv);
	//s->set("drone1-linear-z", (s->get("drone1-z") - prev_drone1_z) * dtInv);

	//s->set("drone2-linear-x", (s->get("drone2-x") - prev_drone2_x) * dtInv);
	//s->set("drone2-linear-y", (s->get("drone2-y") - prev_drone2_y) * dtInv);
	//s->set("drone2-linear-z", (s->get("drone2-z") - prev_drone2_z) * dtInv);

	//s->set("drone3-linear-x", (s->get("drone3-x") - prev_drone3_x) * dtInv);
	//s->set("drone3-linear-y", (s->get("drone3-y") - prev_drone3_y) * dtInv);
	//s->set("drone3-linear-z", (s->get("drone3-z") - prev_drone3_z) * dtInv);

	//s->set("drone4-linear-x", (s->get("drone4-x") - prev_drone4_x) * dtInv);
	//s->set("drone4-linear-y", (s->get("drone4-y") - prev_drone4_y) * dtInv);
	//s->set("drone4-linear-z", (s->get("drone4-z") - prev_drone4_z) * dtInv);
}

Drone6DOFControl::~Drone6DOFControl()
{
	delete m_pBulletPhysics;
}

