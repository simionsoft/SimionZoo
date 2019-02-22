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

#include "transform2d.h"
#include "quaternion.h"

Transform2D::Transform2D()
{
	setIdentity();
}

void Transform2D::setIdentity()
{
	m_translation = Vector2D(0.0, 0.0);
	m_rotation = 0.0;
	m_scale = Vector2D(1.0, 1.0);
	m_depth = 0.0;
}

Vector2D Transform2D::operator*(Vector2D& v)
{
	updateMatrix();
	//Vectors are not affected by translations, the homogeneous coordinate is 0
	Vector3D vector2Das3D(v.x(), v.y(), 0.0);
	Vector3D transformedVec3D= m_matrix * vector2Das3D;
	return Vector2D(transformedVec3D.x(), transformedVec3D.y());
}

Point2D Transform2D::operator*(Point2D& p)
{
	updateMatrix();
	//Points are affected by transalations, the homogeneous coordinate is 1
	Point3D point2Das3D= Point3D(p.x(),p.y(),0.0);
	Point3D vec3d = m_matrix * point2Das3D;
	return Point2D(vec3d.x(), vec3d.y());
}

double* Transform2D::getOpenGLMatrix()
{
	updateMatrix();
	return m_matrix.asArray();
}

void Transform2D::updateMatrix()
{
	Quaternion quat;
	Matrix44 rot, trans, scale;

	quat.fromOrientations(0.0, 0.0, m_rotation); //rotation on the XY plane
	Vector3D transVec = Vector3D(m_translation.x(), m_translation.y(), m_depth);
	trans.setTranslation(transVec);
	Vector3D scaleVec = Vector3D(m_scale.x(), m_scale.y(), 1.0);
	scale.setScale(scaleVec); //scale on the XY plane
	m_matrix = trans * rot * scale;
}


