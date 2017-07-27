#include "quaternion.h"
#include "matrix33.h"
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

	//return asin(2 * m_x*m_z + 2 * m_y*m_w);
}
double Quaternion::pitch() const
{
	if (bUseOrientations()) return m_pitch;

	// pitch (y-axis rotation)
	double t2 = +2.0 * (w() * z() - y() * x());
	t2 = t2 > 1.0 ? 1.0 : t2;
	t2 = t2 < -1.0 ? -1.0 : t2;
	return asin(t2);

	//return atan2(2 * m_x*m_w - 2 * m_y*m_z, 1 - 2 * m_x*m_x - 2 * m_y*m_y);
}
double Quaternion::roll() const
{
	if (bUseOrientations()) return m_roll;

	// roll (x-axis rotation)
	double t0 = +2.0 * (w() * x() + z() * y());
	double t1 = +1.0 - 2.0 * (x() * x() + z()*z());
	return atan2(t0, t1);

	//return atan2(2 * m_z*m_w - 2 * m_x*m_y, 1 - 2 * m_z*m_z - 2 * m_y*m_y);
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
	return sinYaw * sinPitch * cosRoll + cosYaw * cosPitch * sinRoll;
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
	return sinYaw * cosPitch * cosRoll + cosYaw * sinPitch * sinRoll;
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
	return cosYaw * sinPitch * cosRoll - sinYaw * cosPitch * sinRoll;
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
	return cosYaw * cosPitch * cosRoll - sinYaw * sinPitch * sinRoll;
}

void Quaternion::fromOrientations(double yaw, double pitch, double roll)
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
	m_w = cosYaw * cosPitch * cosRoll - sinYaw * sinPitch * sinRoll;
	m_x = sinYaw * sinPitch * cosRoll + cosYaw * cosPitch * sinRoll;
	m_y = sinYaw * cosPitch * cosRoll + cosYaw * sinPitch * sinRoll;
	m_z = cosYaw * sinPitch * cosRoll - sinYaw * cosPitch * sinRoll;
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

void Quaternion::asMatrix33(Matrix33& matrix) const
{
	double wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

	x2 = m_x + m_x;  y2 = m_y + m_y;  z2 = m_z + m_z;
	xx = m_x * x2;       xy = m_x * y2;       xz = m_x * z2;
	yy = m_y * y2;       yz = m_y * z2;       zz = m_z * z2;
	wx = m_w * x2;       wy = m_w * y2;       wz = m_w * z2;

	matrix.x().setX( 1.0 - (yy + zz));
	matrix.x().setY( xy - wz);
	matrix.x().setZ( xz + wy);
	matrix.y().setX( xy + wz);
	matrix.y().setY( 1.0 - (xx + zz));
	matrix.y().setZ( yz - wx);
	matrix.z().setX( xz - wy);
	matrix.z().setY( yz + wx);
	matrix.z().setZ( 1.0 - (xx + yy));
}

Quaternion Quaternion::operator+(const Quaternion quat)
{
	if (bUseOrientations())
		fromOrientations();

	Quaternion result = Quaternion(m_x + quat.m_x,
		m_y + quat.m_y, m_z + quat.m_z, m_w + quat.m_w);
	return result;
}

Quaternion Quaternion::operator-(const Quaternion quat)
{
	if (bUseOrientations())
		fromOrientations();

	Quaternion result = Quaternion(m_x - quat.m_x,
		m_y - quat.m_y, m_z - quat.m_z, m_w - quat.m_w);
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
}
void Quaternion::operator-=(const Quaternion quat)
{
	if (bUseOrientations())
		fromOrientations();

	m_x -= quat.m_x;
	m_y -= quat.m_y;
	m_z -= quat.m_z;
	m_w -= quat.m_w;
}

void Quaternion::operator*= (const Quaternion quat)
{
	if (bUseOrientations())
		fromOrientations();

	Quaternion result;
	result = (*this)*quat;
	(*this) = result;
}
