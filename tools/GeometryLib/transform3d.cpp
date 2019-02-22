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

#include "transform3d.h"
#include "matrix44.h"

Transform3D::Transform3D()
{
	setIdentity();
}

void Transform3D::setIdentity()
{
	m_rotation.setIdentity();
	m_translation = Vector3D(0.0, 0.0, 0.0);
	m_scale = Vector3D(1.0, 1.0, 1.0);
	m_matrix.setIdentity();
}

double* Transform3D::getOpenGLMatrix()
{
	updateMatrix();
	return m_matrix.asArray();
}

void Transform3D::updateMatrix()
{
	if (m_rotation.bUseOrientations())
		m_rotation.fromOrientations();

	Matrix44 rot,trans,scale;

	rot.setRotation(m_rotation);
	trans.setTranslation(m_translation);
	scale.setScale(m_scale);
	m_matrix = trans*rot*scale;
}

Vector3D Transform3D::operator*(Vector3D& v)
{
	updateMatrix();
	//Vectors are not affected by translations, the homogeneous coordinate is 0
	return m_matrix*v;
}

Point3D Transform3D::operator*(Point3D& p)
{
	updateMatrix();
	//Points are affected by transalations, the homogeneous coordinate is 1
	return m_matrix*p;
}