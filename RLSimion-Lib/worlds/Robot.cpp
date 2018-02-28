#include "Robot.h"


Robot::Robot(double mass, const btVector3& pos, btCollisionShape* shape)
	: BulletBody(mass, pos, shape )
{
}

void Robot::updateBulletState(CState *s, const CAction *a, double dt)
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

void Robot::updateYawState(CState* s)
{
	//because this value is calculated in updateBulletVelocities() internally, we don't retrieve
	//it from bullet, but from the locally stored value
	s->set(m_thetaId, m_theta);
}