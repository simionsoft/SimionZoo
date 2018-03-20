#pragma once
#include "bindings.h"
#include "../GeometryLib/transform3d.h"

namespace tinyxml2 { class XMLElement; }

class SceneActor
{
protected:
	Transform3D m_transform;
public:
	SceneActor()= default;
	SceneActor(tinyxml2::XMLElement* pNode);
	virtual ~SceneActor();

	void setTransform();
	void restoreTransform();
	Transform3D& getTransform() { return m_transform; }
	Matrix44 getTransformMatrix() const;

	static SceneActor* load(tinyxml2::XMLElement* pNode);

	virtual void addLocalOffset(Vector3D& offset);
	void addWorldOffset(Vector3D& offset);

	void addRotation(Quaternion& quat);
};

