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

#include "Robot.h"


Robot::Robot(double mass, const btVector3& pos, btCollisionShape* shape)
	: BulletBody(mass, pos, shape )
{
}

void Robot::updateBulletState(State *s, const Action *a, double dt)
{
	double rob_VelX, rob_VelY;

	double omega = a->get(m_omegaId);
	double linear_vel = a->get(m_vId);

	double theta = s->get(m_thetaId);
	theta += omega*dt;

	if (theta > SIMD_2_PI)
		theta -= SIMD_2_PI;
	if (theta < -SIMD_2_PI)
		theta += SIMD_2_PI;

	m_theta= theta;

	rob_VelX = cos(theta)*linear_vel;
	rob_VelY = sin(theta)*linear_vel;

	getBody()->setAngularVelocity(btVector3(0.0, omega, 0.0));
	getBody()->setLinearVelocity(btVector3(rob_VelX, 0.0, rob_VelY));
}

void Robot::updateYawState(State* s)
{
	//because this value is calculated in updateBulletVelocities() internally, we don't retrieve
	//it from bullet, but from the locally stored value
	s->set(m_thetaId, m_theta);
}