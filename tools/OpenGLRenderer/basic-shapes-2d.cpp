#include "stdafx.h"
#include "basic-shapes-2d.h"
#include "material.h"
#include "text.h"
#include "renderer.h"
#include "xml-load.h"

Sprite2D::Sprite2D(string name, Vector2D origin, Vector2D size, double depth, Material* material)
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

	if (pNode->Attribute(XML_ATTR_COORDINATES) 
		&& !strcmp(pNode->Attribute(XML_ATTR_COORDINATES), XML_ATTR_VALUE_AROUND_ORIGIN))
	{
		m_minCoord = Vector2D(-0.5, -0.5);
		m_maxCoord = Vector2D(0.5, 0.5);
	}
	else
	{
		m_minCoord = Vector2D(0.0, 0.0);
		m_maxCoord = Vector2D(1.0, 1.0);
	}
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
	glVertex2d(m_minCoord.x(), m_minCoord.y());
	glTexCoord2d(1.0, 1.0);
	glVertex2d(m_maxCoord.x(), m_minCoord.y());
	glTexCoord2d(1.0, 0.0);
	glVertex2d(m_maxCoord.x(), m_maxCoord.y());
	glTexCoord2d(0.0, 0.0);
	glVertex2d(m_minCoord.x(), m_maxCoord.y());
	glEnd();
}

Meter2D::Meter2D(tinyxml2::XMLElement* pNode): GraphicObject2D(pNode)
{
	m_pMaterial = Material::getInstance(pNode);
}

Meter2D::Meter2D(string name, Vector2D origin, Vector2D size, double depth) 
	: GraphicObject2D(name, origin, size, 0.0, depth) //no rotation allowed
{
	m_pMaterial = new ColorMaterial();

	//text on the progress bar, shifted a bit to the top-right toward the viewer
	m_pText = new Text2D(name + "/text", Vector2D( 0.025, 0.25), 0.1);
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

	m_pText->setTransform();
	m_pText->draw();
	m_pText->restoreTransform();
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