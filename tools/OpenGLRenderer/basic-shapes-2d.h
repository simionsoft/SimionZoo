#pragma once
#include "graphic-object-2d.h"

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
	Material* m_pMaterial= nullptr;
	Range m_valueRange;
	double m_value;
	Text2D *m_pText;
public:
	//the constructor uses normalized coordinates [0,1]
	Meter2D(string name, Vector2D origin, Vector2D size, double depth = 0.0);
	Meter2D(tinyxml2::XMLElement* pNode);
	virtual ~Meter2D();

	void setValueRange(Range range) { m_valueRange = range; }
	void setValue(double value);
	void draw();
};
