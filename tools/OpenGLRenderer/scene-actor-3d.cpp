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

#include "stdafx.h"
#include "scene-actor-3d.h"
#include "xml-load.h"
#include "../GeometryLib/matrix44.h"



SceneActor3D::SceneActor3D(tinyxml2::XMLElement* pNode)
{
	XMLElement* pChild;
	pChild= pNode->FirstChildElement(XML_TAG_TRANSFORM);
	if (pChild)
		XML::load(pChild, m_transform);
}

SceneActor3D::~SceneActor3D()
{
}

void SceneActor3D::setTransform()
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	double *pMatrix = getTransformMatrix().asArray();
	if (pMatrix)
		glMultMatrixd(pMatrix);
}

void SceneActor3D::restoreTransform()
{
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

Matrix44 SceneActor3D::getTransformMatrix() const
{
	Matrix44 mat, rot, scale,trans;

	Vector3D translationVector= m_transform.translation();
	Vector3D scaleVector= m_transform.scale();
	rot.setRotation(m_transform.rotation());
	trans.setTranslation(translationVector);
	scale.setScale(scaleVector);
	mat = trans*rot*scale;

	return mat;
}

void SceneActor3D::addLocalOffset(const Vector3D& offset)
{
	Matrix44 mat = getTransformMatrix();
	Vector3D worldOffset = mat*offset;
	m_transform.setTranslation(m_transform.translation() + worldOffset);
}

void SceneActor3D::addWorldOffset(const Vector3D& offset)
{
	m_transform.setTranslation(	m_transform.translation() + offset);
}

void SceneActor3D::addRotation(const Quaternion& quat)
{
	m_transform.setRotation(m_transform.rotation() * quat);
}

void SceneActor3D::setRotation(const Quaternion& quat)
{
	m_transform.setRotation(quat);
}

Quaternion SceneActor3D::getRotation() const
{
	return m_transform.rotation();
}