#pragma once
#include "graphic-object-2d.h"
#include "color.h"

class Sprite2D : public GraphicObject2D
{
	Vector2D m_minCoord, m_maxCoord;
	Material* m_pMaterial= nullptr;
public:
	Sprite2D(string name, Vector2D origin, Vector2D size, double depth = 0, Material* material= nullptr);
	Sprite2D(tinyxml2::XMLElement* pNode);
	virtual ~Sprite2D();

	virtual void draw();
};

class Meter2D : public GraphicObject2D
{
	Range m_valueRange;
	double m_value;
	Text2D *m_pText;
	Color m_startColor = Color(0.0, 0.0, 1.0);
	Color m_endColor = Color(1.0, 0.0, 0.0);
	Color m_valueColor;
public:
	//the constructor uses normalized coordinates [0,1]
	Meter2D(string name, Vector2D origin, Vector2D size, double depth = 0.0);
	Meter2D(tinyxml2::XMLElement* pNode);
	virtual ~Meter2D();

	void setValueRange(Range range) { m_valueRange = range; }
	void setValue(double value);
	void draw();
};
