#pragma once
#include "transform3d.h"

namespace tinyxml2 { class XMLElement; }

class CActor
{
protected:
	Transform3D m_transform;
public:
	CActor()= default;
	CActor(tinyxml2::XMLElement* pNode);
	virtual ~CActor();

	void setTransform();
	void restoreTransform();
	Transform3D& getTransform() { return m_transform; }
	Matrix44 getTransformMatrix() const;

	static CActor* load(tinyxml2::XMLElement* pNode);

	virtual void addLocalOffset(Vector3D& offset);
	void addWorldOffset(Vector3D& offset);

	void addRotation(Quaternion& quat);
};

