#include "stdafx.h"
#include "scene-actor-2d.h"
#include "xml-load.h"

SceneActor2D::SceneActor2D(tinyxml2::XMLElement* pNode)
{
	XMLElement* pChild;
	pChild = pNode->FirstChildElement(XML_TAG_TRANSFORM);
	if (pChild)
		XML::load(pChild, m_transform);
}

SceneActor2D::SceneActor2D(Vector2D origin, Vector2D size, double rotation, double depth)
{
	m_transform.setTranslation(origin);
	m_transform.setScale(size);
	m_transform.setRotation(rotation);
	m_transform.setDepth(depth);
}

SceneActor2D::~SceneActor2D()
{
}


void SceneActor2D::setTransform()
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	double *pMatrix = getTransformMatrix().asArray();
	if (pMatrix)
		glMultMatrixd(pMatrix);
}

void SceneActor2D::restoreTransform()
{
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

Matrix44 SceneActor2D::getTransformMatrix()
{
	Quaternion quat;
	Matrix44 mat, rot, scale, trans;
	quat.fromOrientations(0.0, 0.0, m_transform.rotation());
	rot.setRotation(quat);
	Vector2D translation= m_transform.translation();
	trans.setTranslation(Vector3D( translation.x(), translation.y(), m_transform.depth()));
	Vector2D s = m_transform.scale();
	scale.setScale(Vector3D(s.x(),s.y(),1.0));
	mat = trans * rot * scale;
	return mat;
}

void SceneActor2D::addLocalOffset(Vector2D& offset)
{
	Matrix44 mat = getTransformMatrix();
	Vector3D worldOffset = mat * Vector3D(offset.x(),offset.y(),0.0);
	m_transform.setTranslation(m_transform.translation() + Vector2D(worldOffset.x(),worldOffset.y()));
}

void SceneActor2D::addWorldOffset(Vector2D& offset)
{
	m_transform.setTranslation(m_transform.translation() + offset);
}

void SceneActor2D::addRotation(double angle)
{
	m_transform.setRotation(m_transform.rotation() + angle);
}