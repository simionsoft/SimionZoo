#include "matrix33.h"
#include "quaternion.h"

Vector3D Matrix33::operator*(Vector3D& vec)
{
	Vector3D result;
	result.setX( vec.x() * m_x.x() + vec.y() * m_y.x() + vec.z() * m_z.x());
	result.setY( vec.x() * m_x.y() + vec.y() * m_y.y() + vec.z() * m_z.y());
	result.setZ( vec.x() * m_x.z() + vec.y() * m_y.z() + vec.z() * m_z.z());
	return result;
}

void Matrix33::setRotation(Quaternion& quat)
{
	double wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

	x2 = quat.x() + quat.x();  y2 = quat.y() + quat.y();  z2 = quat.z() + quat.z();
	xx = quat.x() * x2;       xy = quat.x() * y2;       xz = quat.x() * z2;
	yy = quat.y() * y2;       yz = quat.y() * z2;       zz = quat.z() * z2;
	wx = quat.w() * x2;       wy = quat.w() * y2;       wz = quat.w() * z2;

	m_x.setX(1.0 - (yy + zz));
	m_x.setY(xy - wz);
	m_x.setZ(xz + wy);
	m_y.setX(xy + wz);
	m_y.setY(1.0 - (xx + zz));
	m_y.setZ(yz - wx);
	m_z.setX(xz - wy);
	m_z.setY(yz + wx);
	m_z.setZ(1.0 - (xx + yy));
}