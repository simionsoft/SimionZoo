#pragma once
#include "scene-actor.h"
#include "../GeometryLib/frustum.h"

namespace tinyxml2 { class XMLElement; }

class Camera: public SceneActor
{
protected:
	Frustum m_frustum;
public:
	Camera();
	virtual ~Camera();

	static Camera* getInstance(tinyxml2::XMLElement* pNode);
	virtual void set()= 0;

	static void set2DView();

	Matrix44 getModelviewMatrix() const;
	Frustum& getFrustum(){ return m_frustum; }
};

class SimpleCamera : public Camera
{
	double nearPlane = 1.0, farPlane = 100.0;
public:
	SimpleCamera();
	SimpleCamera(tinyxml2::XMLElement* pNode);
	void set();
};