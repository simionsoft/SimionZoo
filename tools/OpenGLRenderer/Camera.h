#pragma once
#include "scene-actor.h"
#include "../GeometryLib/frustum.h"

namespace tinyxml2 { class XMLElement; }

class CCamera: public CSceneActor
{
protected:
	Frustum m_frustum;
public:
	CCamera();
	virtual ~CCamera();

	static CCamera* getInstance(tinyxml2::XMLElement* pNode);
	virtual void set()= 0;

	static void set2DView();

	Matrix44 getModelviewMatrix() const;
	Frustum& getFrustum(){ return m_frustum; }
};

class CSimpleCamera : public CCamera
{
	double nearPlane = 1.0, farPlane = 100.0;
public:
	CSimpleCamera();
	CSimpleCamera(tinyxml2::XMLElement* pNode);
	void set();
};