#include "stdafx.h"
#include "text.h"
#include "renderer.h"



Text2D::Text2D(string name, Vector2D origin, double depth)
	: m_color(Color(0.0, 0.0, 0.0, 1.0))
	, GraphicObject2D(name,origin,Vector2D(1.0,1.0),0.0, depth) //size 1, no rotation
{
}

Text2D::~Text2D()
{
}

void Text2D::draw()
{
	//set color
	glColor3fv(m_color.rgba());
	
	//according to documentation, passed coordinates are transformed by the modelview matrix
	glRasterPos3d(0.0, 0.0, 0);

	void * font = GLUT_BITMAP_9_BY_15;

	glutBitmapString(font, (const unsigned char*)m_text.c_str());

}
