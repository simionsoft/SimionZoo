#include "stdafx.h"
#include "basic-shapes-2d.h"
#include "material.h"
#include "text.h"
#include "renderer.h"
#include "xml-load.h"

Sprite2D::Sprite2D(string name, Vector2D origin, Vector2D size, int depth, Material* material)
	:GraphicObject2D(name, origin, size, depth)
{
	m_pMaterial = material;
}

Sprite2D::Sprite2D(tinyxml2::XMLElement* pNode)
	: GraphicObject2D(pNode)
{
	tinyxml2::XMLElement* pChild = pNode->FirstChildElement(XML_TAG_MATERIAL);
	if (pChild)
		m_pMaterial = Material::getInstance(pChild->FirstChildElement());
}

Sprite2D::~Sprite2D()
{
	if (m_pMaterial)
		delete m_pMaterial;
}

void Sprite2D::draw()
{
	if (m_pMaterial)
		m_pMaterial->set();

	glBegin(GL_QUADS);
		glTexCoord2d(0.0, 1.0);
		glVertex2d(0.0, 0.0);
		glTexCoord2d(1.0, 1.0);
		glVertex2d(1.0, 0.0);
		glTexCoord2d(1.0, 0.0);
		glVertex2d(1.0, 1.0);
		glTexCoord2d(0.0, 0.0);
		glVertex2d(0.0, 1.0);
	glEnd();
}

Meter2D::Meter2D(tinyxml2::XMLElement* pNode): GraphicObject2D(pNode)
{
	m_pMaterial = Material::getInstance(pNode);
}

Meter2D::Meter2D(string name, Vector2D origin, Vector2D size, int depth) 
	: GraphicObject2D(name,origin,size,depth)
{
	m_pMaterial = new ColorMaterial();

	m_pText = new Text2D(name + "/text", origin, depth + 1); //over the progress bar
	m_pText->addPixelOffset(Vector2D(2, 5));
}

Meter2D::~Meter2D()
{
	if (m_pMaterial != nullptr)
		delete m_pMaterial;
	delete m_pText;
}


void Meter2D::draw()
{
	double normValue = m_valueRange.normPosInRange(m_value);

	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_LINE_STRIP);
		glVertex2d(0.0, 0.0);
		glVertex2d(0.0, 1.0);
		glVertex2d(1.0, 1.0);
		glVertex2d(1.0, 0.0);
		glVertex2d(0.0, 0.0);
	glEnd();

	if (m_pMaterial)
		m_pMaterial->set();

	glBegin(GL_QUADS);
		glVertex2d(0.0, 0.0);
		glVertex2d(normValue, 0.0);
		glVertex2d(normValue, 1.0);
		glVertex2d(0.0, 1.0);
	glEnd();

	m_pText->draw();
}

void Meter2D::setValue(double value)
{
	m_value = m_valueRange.clamp(value);

	double normValue = m_valueRange.normPosInRange(m_value);

	//udpate the color
	((ColorMaterial*)m_pMaterial)->setColor(Color(1.0, 0.0, 0.0, 1.0).lerp(Color(0.0, 1.0, 0.0, 1.0)
		, normValue));
	//update the text
	char buffer[1024];
	sprintf_s(buffer, 1024, "%s: %.4f", m_name.c_str(), value);
	m_pText->set(string(buffer));
}

BoundingBox2D& GraphicObject2D::boundingBox()
{
	return m_bb;
}