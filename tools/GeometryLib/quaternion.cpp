#include "quaternion.h"
#include <math.h>

void Quaternion::fromOrientations()
{
	fromOrientations(m_yaw, m_pitch, m_roll);
}

void Quaternion::setX(const double x) { m_x = x; m_bOrientationsSet = false;}
void Quaternion::setY(const double y) { m_y = y; m_bOrientationsSet = false;}
void Quaternion::setZ(const double z) { m_z = z; m_bOrientationsSet = false;}
void Quaternion::setW(const double w) { m_w = w; m_bOrientationsSet = false;}

//this is not efficient, but so far it's enough
double Quaternion::x() const 
{
	if (m_bOrientationsSet) return xFromOrientations(m_yaw, m_pitch, m_roll);
	return m_x; 
}
double Quaternion::y() const
{
	if (m_bOrientationsSet) return yFromOrientations(m_yaw, m_pitch, m_roll);
	return m_y;
}
double Quaternion::z() const
{ 
	if (m_bOrientationsSet) return zFromOrientations(m_yaw, m_pitch, m_roll);
	return m_z;
}
double Quaternion::w() const
{
	if (m_bOrientationsSet) return wFromOrientations(m_yaw, m_pitch, m_roll);
	return m_w;
}

void Quaternion::setYaw(const double yaw) { m_yaw = yaw; m_bOrientationsSet = true; }
void Quaternion::setPitch(const double pitch) { m_pitch = pitch; m_bOrientationsSet = true;}
void Quaternion::setRoll(const double roll) { m_roll = roll; m_bOrientationsSet = true;}

//http://answers.unity3d.com/questions/416169/finding-pitchrollyaw-from-quaternions.html
//I flipped axis y and z, but only checked yaw()
//https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
double Quaternion::yaw() const
{
	if (bUseOrientations()) return m_yaw;

	double t3 = +2.0 * (w() * y() + x() * z());
	double t4 = +1.0 - 2.0 * (z()*z() + y() * y());
	return atan2(t3, t4);
}
double Quaternion::pitch() const
{
	if (bUseOrientations()) return m_pitch;

	// pitch (y-axis rotation)
	double t0 = +2.0 * (w() * x() + z() * y());
	double t1 = +1.0 - 2.0 * (x() * x() + z()*z());
	return atan2(t0, t1);
}
double Quaternion::roll() const
{
	if (bUseOrientations()) return m_roll;

	// roll (x-axis rotation)
	double t2 = +2.0 * (w() * z() - y() * x());
	t2 = t2 > 1.0 ? 1.0 : t2;
	t2 = t2 < -1.0 ? -1.0 : t2;
	return asin(t2);
}


double Quaternion::xFromOrientations(double yaw, double pitch, double roll)
{
	double halfYaw = yaw * 0.5;
	double halfPitch = pitch * 0.5;
	double halfRoll = roll * 0.5;
	double cosYaw = cos(halfYaw);
	double sinYaw = sin(halfYaw);
	double cosPitch = cos(halfPitch);
	double sinPitch = sin(halfPitch);
	double cosRoll = cos(halfRoll);
	double sinRoll = sin(halfRoll);
	return sinYaw * sinRoll * cosPitch + cosYaw * cosRoll * sinPitch;
}

double Quaternion::yFromOrientations(double yaw, double pitch, double roll)
{
	double halfYaw = yaw * 0.5;
	double halfPitch = pitch * 0.5;
	double halfRoll = roll * 0.5;
	double cosYaw = cos(halfYaw);
	double sinYaw = sin(halfYaw);
	double cosPitch = cos(halfPitch);
	double sinPitch = sin(halfPitch);
	double cosRoll = cos(halfRoll);
	double sinRoll = sin(halfRoll);
	return sinYaw * cosRoll * cosPitch + cosYaw * sinRoll * sinPitch;
}

double Quaternion::zFromOrientations(double yaw, double pitch, double roll)
{
	double halfYaw = yaw * 0.5;
	double halfPitch = pitch * 0.5;
	double halfRoll = roll * 0.5;
	double cosYaw = cos(halfYaw);
	double sinYaw = sin(halfYaw);
	double cosPitch = cos(halfPitch);
	double sinPitch = sin(halfPitch);
	double cosRoll = cos(halfRoll);
	double sinRoll = sin(halfRoll);
	return cosYaw * sinRoll * cosPitch - sinYaw * cosRoll * sinPitch;
}

double Quaternion::wFromOrientations(double yaw, double pitch, double roll)
{
	double halfYaw = yaw * 0.5;
	double halfPitch = pitch * 0.5;
	double halfRoll = roll * 0.5;
	double cosYaw = cos(halfYaw);
	double sinYaw = sin(halfYaw);
	double cosPitch = cos(halfPitch);
	double sinPitch = sin(halfPitch);
	double cosRoll = cos(halfRoll);
	double sinRoll = sin(halfRoll);
	return cosYaw * cosRoll * cosPitch - sinYaw * sinRoll * sinPitch;
}

void Quaternion::fromOrientations(double yaw, double pitch, double roll)
{
	//https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
	//Had to swap pitch and roll
	double halfYaw = yaw * 0.5;
	double halfPitch = pitch * 0.5;
	double halfRoll = roll * 0.5;
	double cosYaw = cos(halfYaw);
	double sinYaw = sin(halfYaw);
	double cosPitch = cos(halfPitch);
	double sinPitch = sin(halfPitch);
	double cosRoll = cos(halfRoll);
	double sinRoll = sin(halfRoll);
	m_x = sinYaw * sinRoll * cosPitch + cosYaw * cosRoll * sinPitch;
	m_y = sinYaw * cosRoll * cosPitch + cosYaw * sinRoll * sinPitch;
	m_z = cosYaw * sinRoll * cosPitch - sinYaw * cosRoll * sinPitch;
	m_w = cosYaw * cosRoll * cosPitch - sinYaw * sinRoll * sinPitch;
	normalize();
}

Quaternion::Quaternion(double yaw, double pitch, double roll)
{
	fromOrientations(yaw, pitch, roll);
}

Quaternion Quaternion::inverse()
{
	if (bUseOrientations())
		fromOrientations();
	
	return Quaternion(m_x, m_y, m_z, -m_w);
}



Quaternion Quaternion::operator+(const Quaternion quat)
{
	if (bUseOrientations())
		fromOrientations();

	Quaternion result = Quaternion(m_x + quat.m_x,
		m_y + quat.m_y, m_z + quat.m_z, m_w + quat.m_w);
	result.normalize();
	return result;
}

Quaternion Quaternion::operator-(const Quaternion quat)
{
	if (bUseOrientations())
		fromOrientations();

	Quaternion result = Quaternion(m_x - quat.m_x,
		m_y - quat.m_y, m_z - quat.m_z, m_w - quat.m_w);
	result.normalize();
	return result;
}

Quaternion Quaternion::operator*(const Quaternion quat)
{
	if (bUseOrientations())
		fromOrientations();

	Quaternion result;
	result.m_x = m_w * quat.m_x + m_x * quat.m_w + m_y * quat.m_z - m_z * quat.m_y;
	result.m_y = m_w * quat.m_y + m_y * quat.m_w + m_z * quat.m_x - m_x * quat.m_z;
	result.m_z = m_w * quat.m_z + m_z * quat.m_w + m_x * quat.m_y - m_y * quat.m_x;
	result.m_w = m_w * quat.m_w - m_x * quat.m_x - m_y * quat.m_y - m_z * quat.m_z;
	result.normalize();
	return result;
}
void Quaternion::operator+=(const Quaternion quat)
{
	if (bUseOrientations())
		fromOrientations();

	m_x += quat.m_x;
	m_y += quat.m_y;
	m_z += quat.m_z;
	m_w += quat.m_w;
	normalize();
}
void Quaternion::operator-=(const Quaternion quat)
{
	if (bUseOrientations())
		fromOrientations();

	m_x -= quat.m_x;
	m_y -= quat.m_y;
	m_z -= quat.m_z;
	m_w -= quat.m_w;
	normalize();
}

void Quaternion::operator*= (const Quaternion quat)
{
	if (bUseOrientations())
		fromOrientations();

	Quaternion result;
	result = (*this)*quat;
	result.normalize();
	(*this) = result;
}

Quaternion Quaternion::operator*(const double x)
{
	Quaternion result;
	result.m_x *= x;
	result.m_y *= x;
	result.m_z *= x;
	result.m_w *= x;
	return result;
}

void Quaternion::operator*=(const double x)
{
	m_x *= x;
	m_y *= x;
	m_z *= x;
	m_w *= x;
}

void Quaternion::normalize()
{
	double	dist, square;

	square = m_x * m_x + m_y * m_y
		+ m_z * m_z + m_w * m_w;

	if (square > 0.0)
	{
		dist = 1.0 / sqrt(square);
		*this*=dist;
	}
}