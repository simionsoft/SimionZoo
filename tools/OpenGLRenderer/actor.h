#pragma once
#include "transform3d.h"

namespace tinyxml2 { class XMLElement; }

class CSceneActor
{
protected:
	Transform3D m_transform;
public:
	CSceneActor()= default;
	CSceneActor(tinyxml2::XMLElement* pNode);
	virtual ~CSceneActor();

	void setTransform();
	void restoreTransform();
	Transform3D& getTransform() { return m_transform; }
	Matrix44 getTransformMatrix() const;

	static CSceneActor* load(tinyxml2::XMLElement* pNode);

	virtual void addLocalOffset(Vector3D& offset);
	void addWorldOffset(Vector3D& offset);

	void addRotation(Quaternion& quat);
};

