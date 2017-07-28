#pragma once

#include "vector2d.h"
#include "quaternion.h"

//2D objects are drawn on the z-plane
//By default, the depth is set to z= 0, but different values can be used to allow overlays

class Transform2D
{
protected:
	double m_matrix44[16];
protected:
	double m_depth;
	Vector2D m_translation;
	double m_rotation;
	Vector2D m_scale;

public:
	Transform2D();
	virtual ~Transform2D() {}

	Vector2D& translation() { return m_translation; }
	double& rotation() { return m_rotation; }
	Vector2D& scale() { return m_scale; }
	double& depth() { return m_depth; }

	void setTranslation(const Vector2D& translation) { m_translation = translation; }
	void setRotation(double rotation) { m_rotation = rotation; }
	void setScale(const Vector2D& scale) { m_scale = scale; }
	void setDepth(double depth) { m_depth = depth; }

	double* getOpenGLMatrix();
};

