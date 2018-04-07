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
	Vector3D vec3d= m_matrix * Vector3D(v.x(),v.y(),0.0);
	return Vector2D(vec3d.x(), vec3d.y());
}

Point2D Transform2D::operator*(Point2D& p)
{
	updateMatrix();
	//Points are affected by transalations, the homogeneous coordinate is 1
	Point3D vec3d= m_matrix * Point3D(p.x(),p.y(),0.0);
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
	trans.setTranslation(Vector3D(m_translation.x(),m_translation.y(),m_depth));
	scale.setScale(Vector3D(m_scale.x(),m_scale.y(),1.0)); //scale on the XY plane
	m_matrix = scale * rot * trans;
}


