#include "stdafx.h"
#include "text.h"
#include "renderer.h"
#include "xml-load.h"


Text2D::Text2D(string name, Vector2D origin, double depth)
	: GraphicObject2D(name, origin, Vector2D(1.0, 1.0), 0.0, depth) //size 1, no rotation
	, m_color(Color(0.0, 0.0, 0.0, 1.0))
{
}

Text2D::Text2D(tinyxml2::XMLElement* pNode)
	:GraphicObject2D(pNode)
{
	m_text = pNode->Attribute(XML_ATTR_VALUE);
	if (pNode->FirstChildElement(XML_TAG_COLOR))
		XML::load(pNode, m_color);
}

Text2D::~Text2D()
{
}

void Text2D::draw()
{
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glDisable(GL_BLEND);
	//set color
	glColor3fv(m_color.rgba());
	
	//according to documentation, passed coordinates are transformed by the modelview matrix
	glRasterPos3d(0.0, 0.0, 0);

	void * font = GLUT_BITMAP_HELVETICA_10;

	glutBitmapString(font, (const unsigned char*)m_text.c_str());

}
