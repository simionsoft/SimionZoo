#include "transform3d.h"
#include "matrix33.h"
#include "matrix44.h"

Transform3D::Transform3D()
{
	m_rotation.setIdentity();
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
	m_matrix = rot*trans;
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