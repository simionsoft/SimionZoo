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

#include "underwatervehicle.h"
#include "../../Common/named-var-set.h"
#include "setpoint.h"
#include "../config.h"
#include "world.h"
#include "../app.h"
#include "../reward.h"

#include <math.h>

UnderwaterVehicle::UnderwaterVehicle(ConfigNode* pConfigNode)
{
	METADATA("World", "Underwater-vehicle");
	m_sVSetpoint = addStateVariable("v-setpoint","m/s",-5.0,5.0);
	m_sV = addStateVariable("v","m/s",-5.0,5.0);
	m_sVDeviation = addStateVariable("v-deviation","m/s",-10.0,10.0);

	m_aUThrust = addActionVariable("u-thrust","N",-30.0,30.0);

	FILE_PATH_PARAM setpointFile= FILE_PATH_PARAM(pConfigNode, "Set-Point-File"
		,"The setpoint file", "../config/world/underwater-vehicle/setpoint.txt");

	m_pSetpoint= new FileSetPoint(setpointFile.get());

	m_pRewardFunction->addRewardComponent(new ToleranceRegionReward("v-deviation", 0.1, 1.0));
	m_pRewardFunction->initialize();
}

UnderwaterVehicle::~UnderwaterVehicle()
{
	delete m_pSetpoint;
}



void UnderwaterVehicle::reset(State *s)
{
	s->set(m_sVSetpoint,m_pSetpoint->getPointSet(0.0));
	s->set(m_sVDeviation,m_pSetpoint->getPointSet(0.0));
	s->set(m_sV,0.0);
}

void UnderwaterVehicle::executeAction(State *s,const Action *a,double dt)
{
	double newSetpoint = m_pSetpoint->getPointSet(SimionApp::get()->pWorld->getEpisodeSimTime());
	double v= s->get(m_sV);
	double u= a->get(m_aUThrust); //thrust
	double dot_v= (u*(-0.5*tanh((fabs((1.2+0.2*sin(fabs(v)))*v*fabs(v) - u) -30.0)*0.1) + 0.5) 
		- (1.2+0.2*sin(fabs(v)))*v*fabs(v))	/(3.0+1.5*sin(fabs(v)));
	double newV= v + dot_v*dt;

	s->set(m_sV,newV);
	s->set(m_sVSetpoint,newSetpoint);
	s->set(m_sVDeviation,newSetpoint-newV);
}
