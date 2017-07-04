#include "stdafx.h"
#include "text.h"
#include "renderer.h"



C2DText::C2DText(string name, Vector2D origin, int depth)
	: m_color(Color(0.0, 0.0, 0.0, 1.0)), CGraphicObject2D(name)
{
	int screenWidth, screenHeight;

	CRenderer::get()->getWindowsSize(screenWidth, screenHeight);
	m_absPosX = (int) (origin.x()*screenWidth);
	m_absPosY = (int) (origin.y()*screenHeight);

	m_transform.setTranslation(Vector2D(m_absPosX, m_absPosY));
	m_transform.setScale(Vector2D(1.0,1.0));
	m_transform.setDepth(depth);
}

C2DText::~C2DText()
{
}

void C2DText::draw()
{
	//set color
	glColor3fv(m_color.rgba());
	
	glRasterPos3i((int)m_transform.translation().x()
		, (int)m_transform.translation().y(), (int) m_transform.depth());

	void * font = GLUT_BITMAP_9_BY_15;

	glutBitmapString(font, (const unsigned char*)m_text.c_str());

}

void C2DText::addPixelOffset(const Vector2D offset)
{
	m_transform.setTranslation( m_transform.translation()+offset );
}