#pragma once
#include "vector3d.h"
#include "quaternion.h"
#include "matrix44.h"

class Transform3D
{
protected:
	Vector3D m_translation;
	Quaternion m_rotation;
	Vector3D m_scale;
	Matrix44 m_matrix;

	void updateMatrix();
public:

	Transform3D();
	virtual ~Transform3D() {}

	Vector3D operator*(Vector3D& v);
	Point3D operator*(Point3D& v);

	Vector3D& translation() { return m_translation; }
	Quaternion& rotation() { return m_rotation; }
	Vector3D& scale() { return m_scale; }
	Vector3D translation() const { return m_translation; }
	Quaternion rotation() const { return m_rotation; }
	Vector3D scale() const { return m_scale; }
	Matrix44 transformMatrix() { updateMatrix(); return m_matrix; }

	void setTranslation(const Vector3D& translation) { m_translation = translation; }
	void setRotation(const Quaternion& rotation) { m_rotation = rotation; }
	void setScale(const Vector3D scale) { m_scale = scale; }

	double* getOpenGLMatrix();
};