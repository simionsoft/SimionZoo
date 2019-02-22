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

#include "vector3d.h"
#include <math.h>

Vector3D Vector3D::operator+(const Vector3D& vec) const
{
	return Vector3D(m_x + vec.m_x, m_y + vec.m_y, m_z + vec.m_z);
}

Vector3D Vector3D::operator*(const double scalar) const
{
	return Vector3D(m_x*scalar, m_y*scalar, m_z*scalar);
}

Vector3D Vector3D::operator/(const double scalar) const
{
	if (scalar!=0.0)
		return Vector3D(m_x/scalar, m_y/scalar, m_z/scalar);
	return Vector3D(0.0, 0.0, 0.0);
}

Vector3D Vector3D::operator*(const Vector3D& v) const
{
	return Vector3D(m_x*v.x(), m_y*v.y(), m_z*v.z());
}

Vector3D Vector3D::operator-(const Vector3D& vec) const
{
	return Vector3D(m_x - vec.m_x, m_y - vec.m_y, m_z - vec.m_z);
}

Vector3D& Vector3D::operator+=(const Vector3D& vec)
{
	m_x += vec.m_x;
	m_y += vec.m_y;
	m_z += vec.m_z;
	return (*this);
}

Vector3D& Vector3D::operator*=(const double scalar)
{
	m_x *= scalar;
	m_y *= scalar;
	m_z *= scalar;
	return (*this);
}

Vector3D& Vector3D::operator/=(const double scalar)
{
	if (scalar != 0.0)
	{
		m_x *= scalar;
		m_y *= scalar;
		m_z *= scalar;
	}
	return (*this);
}


Vector3D& Vector3D::operator*=(const Vector3D& v)
{
	m_x *= v.x();
	m_y *= v.y();
	m_z *= v.z();
	return (*this);
}

Vector3D& Vector3D::operator-=(const Vector3D& vec)
{
	m_x -= vec.m_x;
	m_y -= vec.m_y;
	m_z -= vec.m_z;
	return (*this);
}

Vector3D Vector3D::cross(const Vector3D& vec) const
{
	return Vector3D(
		m_y * vec.m_z - m_z * vec.m_y,
		m_z * vec.m_x - m_x * vec.m_z,
		m_x * vec.m_y - m_y * vec.m_x);
}

Vector3D Vector3D::inverse() const
{
	return Vector3D(-m_x, -m_y, -m_z);
}

double Vector3D::dot(const Vector3D& vec) const
{
	return m_x*vec.m_x + m_y* vec.m_y + m_z*vec.m_z;
}

void Vector3D::normalize()
{
	double l = length();
	double invLength;
	if (l == 0.0) return;
	invLength = 1.0 / l;
	m_x = m_x*invLength;
	m_y = m_y*invLength;
	m_z = m_z*invLength;
}

double Vector3D::length() const
{
	return sqrt(m_x*m_x + m_y*m_y + m_z*m_z);
}

Vector3D Vector3D::lerp(const Vector3D &vec1, const Vector3D &vec2, double u)
{
	return Vector3D(vec1 + (vec2 - vec1)*u);
}