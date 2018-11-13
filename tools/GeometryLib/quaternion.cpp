#include "quaternion.h"
#define _USE_MATH_DEFINES
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

//http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/index.htm
double Quaternion::yaw() const
{
	if (bUseOrientations()) return m_yaw;

	// yaw (y-axis rotation)
	double test = x()*y() + z()*w();
	if (test > 0.499) // singularity at north pole
		return 2 * atan2(x(), w());
	
	if (test < -0.499) // singularity at south pole
		return -2 * atan2(x(), w());

	double sqy = y()*y();
	double sqz = z()*z();
	return atan2(2 * y()*w() - 2 * x()*z(), 1 - 2 * sqy - 2 * sqz);
}
double Quaternion::pitch() const
{
	if (bUseOrientations()) return m_pitch;

	// pitch (x-axis rotation)
	double test = x()*y() + z()*w();
	if (test > 0.499) // singularity at north pole
		return 0.0;

	if (test < -0.499) // singularity at south pole
		return 0.0;

	double sqx = x()*x();
	double sqz = z()*z();
	return atan2(2 * x()*w() - 2 * y()*z(), 1 - 2 * sqx - 2 * sqz);
}
double Quaternion::roll() const
{
	if (bUseOrientations()) return m_roll;

	// roll (z-axis rotation)
	double test = x()*y() + z()*w();
	if (test > 0.499) // singularity at north pole
		return M_PI * 0.5;

	if (test < -0.499) // singularity at south pole
		return -M_PI * 0.5;

	return asin(2 * test);
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

//http://www.euclideanspace.com/maths/geometry/rotations/conversions/eulerToQuaternion/index.htm
void Quaternion::fromOrientations(double yaw, double pitch, double roll)
{
	//angles as in: http://www.euclideanspace.com/maths/standards/index.htm
	double halfHeading = yaw * 0.5;
	double halfBank = pitch * 0.5;
	double halfAttitude = roll * 0.5;
	
	double c1 = cos(halfHeading);
    double s1 = sin(halfHeading);
    double c2 = cos(halfAttitude);
    double s2 = sin(halfAttitude);
    double c3 = cos(halfBank);
    double s3 = sin(halfBank);
    double c1c2 = c1*c2;
    double s1s2 = s1*s2;
    m_w =c1c2*c3 - s1s2*s3;
  	m_x =c1c2*s3 + s1s2*c3;
	m_y =s1*c2*c3 + c1*s2*s3;
	m_z =c1*s2*c3 - s1*c2*s3;

	normalize();

	m_bOrientationsSet = false;
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
	if (bUseOrientations())
		fromOrientations();

	Quaternion result;
	result.m_x *= x;
	result.m_y *= x;
	result.m_z *= x;
	result.m_w *= x;
	return result;
}

void Quaternion::operator*=(const double x)
{
	if (bUseOrientations())
		fromOrientations();

	m_x *= x;
	m_y *= x;
	m_z *= x;
	m_w *= x;
}

double Quaternion::squareLength()
{
	return m_x * m_x + m_y * m_y + m_z * m_z + m_w * m_w;
}

double Quaternion::length()
{
	return sqrt(squareLength());
}

void Quaternion::normalize()
{
	double	dist, square;

	square = squareLength();

	if (square > 0.0)
	{
		dist = 1.0 / sqrt(square);
		//don't call to Quaternion::operator*= as it would make the recursivity infinite
		m_x *= dist;
		m_y *= dist;
		m_z *= dist;
		m_w *= dist;
	}
}