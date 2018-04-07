#pragma once

#include <string.h>
using namespace std;
#include "../GeometryLib/transform2d.h"

class SceneActor2D
{
	Transform2D m_transform;
public:
	SceneActor2D() = default;
	virtual ~SceneActor2D();
	SceneActor2D(tinyxml2::XMLElement* pNode);

	void setTransform();
	void restoreTransform();
	Transform2D& getTransform() { return m_transform; }
	Matrix44 getTransformMatrix();

	virtual void addLocalOffset(Vector2D& offset);
	void addWorldOffset(Vector2D& offset);
	void addRotation(double angle);
};

