#include "stdafx.h"
#include "actor.h"
#include "xml-load-utils.h"
#include "matrix44.h"



CActor::CActor(tinyxml2::XMLElement* pNode)
{
	XMLElement* pChild;
	pChild= pNode->FirstChildElement(XML_TAG_TRANSFORM);
	if (pChild)
		m_transform.load(pChild);
}

CActor::~CActor()
{
}

void CActor::setTransform()
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	double *pMatrix = getTransformMatrix().asArray();
	if (pMatrix)
		glMultMatrixd(pMatrix);
}

void CActor::restoreTransform()
{
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

Matrix44 CActor::getTransformMatrix() const
{
	Matrix44 mat, rot, scale,trans;
	rot = Matrix44::rotationMatrix(m_transform.rotation());
	trans = Matrix44::translationMatrix(m_transform.translation());
	scale = Matrix44::scaleMatrix(m_transform.scale());
	mat = trans*rot*scale;
	return mat;
}

void CActor::addLocalOffset(Vector3D& offset)
{
	Matrix44 mat = getTransformMatrix();
	Vector3D worldOffset = mat*offset;
	m_transform.setTranslation(m_transform.translation() + worldOffset);
}

void CActor::addWorldOffset(Vector3D& offset)
{
	m_transform.setTranslation(	m_transform.translation() + offset);
}

void CActor::addRotation(Quaternion& quat)
{
	m_transform.setRotation(m_transform.rotation() * quat);
}