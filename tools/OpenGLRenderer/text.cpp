/*
	SimionZoo: A framework for online model-free Reinforcement Learning on continuous
	control problems

	Copyright (c) 2016 SimionSoft. https://github.com/simionsoft

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

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
