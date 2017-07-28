#pragma once

#include "../GeometryLib/vector3d.h"
#include "color.h"

namespace tinyxml2 { class XMLElement; }

class CLight
{
protected:
	int m_id = 0;
	Color m_diffuseColor, m_specularColor, m_ambientColor;
public:
	CLight(tinyxml2::XMLElement* pNode);
	CLight(): m_diffuseColor(0.25f, 0.25f, 0.25f, 1.0f)
		, m_specularColor(0.25f, 0.25f, 0.25f, 1.0f)
		, m_ambientColor(0.1f, 0.1f, 0.1f, 1.0f) {}
	virtual ~CLight();

	static void enable(bool enable);
	static CLight* getInstance(tinyxml2::XMLElement* pNode);

	virtual void set()= 0;

	void setId(int id) { m_id = id; }
};

class CDirectionalLight : public CLight
{
	Vector3D m_direction;
public:
	CDirectionalLight() : m_direction(Vector3D(0.0, -1.0, 0.0)), CLight(){}
	CDirectionalLight(tinyxml2::XMLElement* pNode);

	void setDirection(Vector3D dir) { m_direction= dir; }
	Vector3D getDirection() { return m_direction; }
	void setColor(Color color) { m_diffuseColor = m_specularColor= m_ambientColor= color; }
	Color getDiffuseColor() { return m_diffuseColor; }

	void set();
};