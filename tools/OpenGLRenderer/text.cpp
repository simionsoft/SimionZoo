#include "stdafx.h"
#include "text.h"
#include "renderer.h"



Text2D::Text2D(string name, Vector2D origin, int depth)
	: m_color(Color(0.0, 0.0, 0.0, 1.0)), GraphicObject2D(name)
{
	int screenWidth, screenHeight;

	Renderer::get()->getWindowsSize(screenWidth, screenHeight);
	m_absPosX = (int) (origin.x()*screenWidth);
	m_absPosY = (int) (origin.y()*screenHeight);

	m_transform.setTranslation(Vector2D(m_absPosX, m_absPosY));
	m_transform.setScale(Vector2D(1.0,1.0));
	m_transform.setDepth(depth);
}

Text2D::~Text2D()
{
}

void Text2D::draw()
{
	//set color
	glColor3fv(m_color.rgba());
	
	glRasterPos3i((int)m_transform.translation().x()
		, (int)m_transform.translation().y(), (int) m_transform.depth());

	void * font = GLUT_BITMAP_9_BY_15;

	glutBitmapString(font, (const unsigned char*)m_text.c_str());

}

void Text2D::addPixelOffset(const Vector2D offset)
{
	m_transform.setTranslation( m_transform.translation()+offset );
}