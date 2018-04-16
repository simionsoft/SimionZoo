#pragma once
#include "../GeometryLib/transform3d.h"

namespace tinyxml2 { class XMLElement; }

class SceneActor3D
{
protected:
	Transform3D m_transform;
public:
	SceneActor3D()= default;
	SceneActor3D(tinyxml2::XMLElement* pNode);
	virtual ~SceneActor3D();

	void setTransform();
	void restoreTransform();
	Transform3D& getTransform() { return m_transform; }
	Matrix44 getTransformMatrix() const;

	virtual void addLocalOffset(Vector3D& offset);
	void addWorldOffset(Vector3D& offset);

	void addRotation(Quaternion& quat);
	void setRotation(Quaternion& quat);
	Quaternion getRotation() const;
};

