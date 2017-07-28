#include "matrix44.h"


Matrix44::Matrix44()
{
}


Matrix44::~Matrix44()
{
}

void Matrix44::setIdentity()
{
	set(0, 0, 1.0); set(0, 1, 0.0); set(0, 2, 0.0); set(0, 3, 0.0);
	set(1, 0, 0.0); set(1, 1, 1.0); set(1, 2, 0.0); set(1, 3, 0.0);
	set(2, 0, 0.0); set(2, 1, 0.0); set(2, 2, 1.0); set(2, 3, 0.0);
	set(3, 0, 0.0); set(3, 1, 0.0); set(3, 2, 0.0); set(3, 3, 1.0);
}

void Matrix44::setRotation(Quaternion& quat)
{
	double wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

	x2 = quat.x() + quat.x();  y2 = quat.y() + quat.y();  z2 = quat.z() + quat.z();
	xx = quat.x() * x2;       xy = quat.x() * y2;       xz = quat.x() * z2;
	yy = quat.y() * y2;       yz = quat.y() * z2;       zz = quat.z() * z2;
	wx = quat.w() * x2;       wy = quat.w() * y2;       wz = quat.w() * z2;
	
	set(0, 0, 1.0 - (yy + zz));
	set(0, 1, xy - wz);
	set(0, 2, xz + wy);
	set(0, 3, 0.0);
	set(1, 0, xy + wz);
	set(1, 1, 1.0 - (xx + zz));
	set(1, 2, yz - wx);
	set(1, 3, 0.0);
	set(2, 0, xz - wy);
	set(2, 1, yz + wx);
	set(2, 2, 1.0 - (xx + yy));
	set(2, 3, 0.0);
	set(3, 0, 0.0);
	set(3, 1, 0.0);
	set(3, 2, 0.0);
	set(3, 3, 1.0);
}

void Matrix44::setTranslation(Vector3D& translation)
{
	set(0, 0, 1.0);
	set(0, 1, 0.0);
	set(0, 2, 0.0);
	set(0, 3, 0.0);
	set(1, 0, 0.0);
	set(1, 1, 1.0);
	set(1, 2, 0.0);
	set(1, 3, 0.0);
	set(2, 0, 0.0);
	set(2, 1, 0.0);
	set(2, 2, 1.0);
	set(2, 3, 0.0);
	set(3, 0, translation.x());
	set(3, 1, translation.y());
	set(3, 2, translation.z());
	set(3, 3, 1.0);
}
void Matrix44::setScale(Vector3D& scale)
{
	set(0, 0, scale.x());
	set(0, 1, 0.0);
	set(0, 2, 0.0);
	set(0, 3, 0.0);
	set(1, 0, 0.0);
	set(1, 1, scale.y());
	set(1, 2, 0.0);
	set(1, 3, 0.0);
	set(2, 0, 0.0);
	set(2, 1, 0.0);
	set(2, 2, scale.z());
	set(2, 3, 0.0);
	set(3, 0, 0.0);
	set(3, 1, 0.0);
	set(3, 2, 0.0);
	set(3, 3, 1.0);
}

void Matrix44::setPerspective(double halfWidth, double halfHeight, double nearPlaneDist, double farPlaneDist)
{
	//http://www.songho.ca/opengl/gl_projectionmatrix.html
	setIdentity();
	
	if (farPlaneDist - nearPlaneDist == 0.0) return;

	//[0,0]= n/r
	set(0, 0, nearPlaneDist / halfWidth);
	//[1,1]= n/t
	set(1, 1, nearPlaneDist / halfHeight);
	//[2,2]= -(f+n)/(f-n)
	set(2, 2, -(farPlaneDist + nearPlaneDist) / (farPlaneDist - nearPlaneDist));
	//[3,2]= -2fn/(f-n)
	set(3, 2, -2 * farPlaneDist*nearPlaneDist / (farPlaneDist - nearPlaneDist));
	//[2,3]= -1
	set(2, 3, -1.0);
	//[3,3]= 0
	set(3, 3, 0.0);
}

BoundingBox3D Matrix44::operator*(const BoundingBox3D& box) const
{
	BoundingBox3D result;
	result = BoundingBox3D(*this*box.min(), *this*box.max());
	return result;
}

Matrix44 Matrix44::operator*(Matrix44& mat) const
{
	Matrix44 result;
	result.set(0, 0, get(0, 0)*mat.get(0, 0) + get(1, 0)*mat.get(0, 1) + get(2, 0)*mat.get(0, 2) + get(3, 0)*mat.get(0, 3));
	result.set(1, 0, get(0, 0)*mat.get(1, 0) + get(1, 0)*mat.get(1, 1) + get(2, 0)*mat.get(1, 2) + get(3, 0)*mat.get(1, 3));
	result.set(2, 0, get(0, 0)*mat.get(2, 0) + get(1, 0)*mat.get(2, 1) + get(2, 0)*mat.get(2, 2) + get(3, 0)*mat.get(2, 3));
	result.set(3, 0, get(0, 0)*mat.get(3, 0) + get(1, 0)*mat.get(3, 1) + get(2, 0)*mat.get(3, 2) + get(3, 0)*mat.get(3, 3));

	result.set(0, 1, get(0, 1)*mat.get(0, 0) + get(1, 1)*mat.get(0, 1) + get(2, 1)*mat.get(0, 2) + get(3, 1)*mat.get(0, 3));
	result.set(1, 1, get(0, 1)*mat.get(1, 0) + get(1, 1)*mat.get(1, 1) + get(2, 1)*mat.get(1, 2) + get(3, 1)*mat.get(1, 3));
	result.set(2, 1, get(0, 1)*mat.get(2, 0) + get(1, 1)*mat.get(2, 1) + get(2, 1)*mat.get(2, 2) + get(3, 1)*mat.get(2, 3));
	result.set(3, 1, get(0, 1)*mat.get(3, 0) + get(1, 1)*mat.get(3, 1) + get(2, 1)*mat.get(3, 2) + get(3, 1)*mat.get(3, 3));

	result.set(0, 2, get(0, 2)*mat.get(0, 0) + get(1, 2)*mat.get(0, 1) + get(2, 2)*mat.get(0, 2) + get(3, 2)*mat.get(0, 3));
	result.set(1, 2, get(0, 2)*mat.get(1, 0) + get(1, 2)*mat.get(1, 1) + get(2, 2)*mat.get(1, 2) + get(3, 2)*mat.get(1, 3));
	result.set(2, 2, get(0, 2)*mat.get(2, 0) + get(1, 2)*mat.get(2, 1) + get(2, 2)*mat.get(2, 2) + get(3, 2)*mat.get(2, 3));
	result.set(3, 2, get(0, 2)*mat.get(3, 0) + get(1, 2)*mat.get(3, 1) + get(2, 2)*mat.get(3, 2) + get(3, 2)*mat.get(3, 3));

	result.set(0, 3, get(0, 3)*mat.get(0, 0) + get(1, 3)*mat.get(0, 1) + get(2, 3)*mat.get(0, 2) + get(3, 3)*mat.get(0, 3));
	result.set(1, 3, get(0, 3)*mat.get(1, 0) + get(1, 3)*mat.get(1, 1) + get(2, 3)*mat.get(1, 2) + get(3, 3)*mat.get(1, 3));
	result.set(2, 3, get(0, 3)*mat.get(2, 0) + get(1, 3)*mat.get(2, 1) + get(2, 3)*mat.get(2, 2) + get(3, 3)*mat.get(2, 3));
	result.set(3, 3, get(0, 3)*mat.get(3, 0) + get(1, 3)*mat.get(3, 1) + get(2, 3)*mat.get(3, 2) + get(3, 3)*mat.get(3, 3));

	return result;
}



Point3D Matrix44::operator*(Point3D& v) const
{
	Point3D result;
	result.setX(get(0, 0)*v.x() + get(1, 0)*v.y() + get(2, 0)*v.z() + get(3, 0));
	result.setY(get(0, 1)*v.x() + get(1, 1)*v.y() + get(2, 1)*v.z() + get(3, 1));
	result.setZ(get(0, 2)*v.x() + get(1, 2)*v.y() + get(2, 2)*v.z() + get(3, 2));
	return result;
}

Vector3D Matrix44::operator*(Vector3D& v) const
{
	Vector3D result;
	result.setX(get(0, 0)*v.x() + get(1, 0)*v.y() + get(2, 0)*v.z());
	result.setY(get(0, 1)*v.x() + get(1, 1)*v.y() + get(2, 1)*v.z());
	result.setZ(get(0, 2)*v.x() + get(1, 2)*v.y() + get(2, 2)*v.z());
	return result;
}
double* Matrix44::asArray()
{
	return _values;
}

double Matrix44::get(int col, int row) const
{
	if (col >= 0 && col < 4 && row>=0 && row < 4)
	{
		return _values[col*4 + row];
	}
	return 0.0;
}

void Matrix44::set(int col, int row, double value)
{
	if (col >= 0 && col < 4 && row>=0 && row < 4)
	{
		_values[col*4 + row]= value;
	}
}

void Matrix44::flipYZAxis()
{
	//https://www.gamedev.net/topic/537664-handedness/#entry4480987
	double tmp;
	//swap 2nd and 3rd row vectors
	for (int i = 0; i < 4; ++i)
	{
		tmp = get(i, 1);
		set(i, 1, get(i, 2));
		set(i, 2, tmp);
	}
	//swap 2nd and 3rd column vectors
	for (int i = 0; i < 4; ++i)
	{
		tmp = get(1, i);
		set(1, i, get(2, i));
		set(2, i, tmp);
	}
}