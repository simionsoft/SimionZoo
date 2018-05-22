#pragma once

#include "color.h"
#include "graphic-object-2d.h"

#include <string>
using namespace std;

class Text2D: public GraphicObject2D
{
	Color m_color;
	string m_text;
public:
	Text2D(tinyxml2::XMLElement* pNode);

	Text2D(string name,Vector2D origin, double depth);
	virtual ~Text2D();

	void set(string text) { m_text = text; }

	void setColor(Color color) { m_color = color; }
	void draw();
};

