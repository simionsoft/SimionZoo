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
	Quaternion rotationQuaternion= m_transform.rotation();
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