#pragma once
#include "actor.h"

namespace tinyxml2 { class XMLElement; }

class CCamera: public CActor
{
public:
	CCamera();
	virtual ~CCamera();

	static CCamera* getInstance(tinyxml2::XMLElement* pNode);
	virtual void set()= 0;

	static void set2DView();

	Matrix44 getModelviewMatrix() const;
};

class CSimpleCamera : public CCamera
{
	double nearPlane = 1.0, farPlane = 100.0;
public:
	CSimpleCamera();
	CSimpleCamera(tinyxml2::XMLElement* pNode);
	void set();
};