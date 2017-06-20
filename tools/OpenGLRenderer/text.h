#pragma once

#include "color.h"
#include "graphic-object-2d.h"

#include <string>
using namespace std;

class C2DText: public CGraphicObject2D
{
	int m_absPosX, m_absPosY;
	Color m_color;
	string m_text;
public:
	C2DText(string name,Vector2D origin, int depth);
	virtual ~C2DText();

	void set(string text) { m_text = text; }

	void setColor(Color color) { m_color = color; }
	void draw();
	void addPixelOffset(const Vector2D offset);
};

