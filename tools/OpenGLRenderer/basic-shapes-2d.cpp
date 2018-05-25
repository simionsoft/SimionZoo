#include "stdafx.h"
#include "basic-shapes-2d.h"
#include "material.h"
#include "text.h"
#include "renderer.h"
#include "xml-load.h"
#include <algorithm>

Sprite2D::Sprite2D(string name, Vector2D origin, Vector2D size, double depth, Material* material)
	:GraphicObject2D(name, origin, size, 0.0, depth)
{
	m_pMaterial = material;
	m_minCoord = Point2D(0.0, 0.0);
	m_maxCoord = Point2D(1.0, 1.0);
}

Sprite2D::Sprite2D(tinyxml2::XMLElement* pNode)
	: GraphicObject2D(pNode)
{
	tinyxml2::XMLElement* pChild = pNode->FirstChildElement(XML_TAG_MATERIAL);
	if (pChild)
		m_pMaterial = Material::getInstance(pChild->FirstChildElement());

	m_minCoord = Vector2D(0.0, 0.0);
	m_maxCoord = Vector2D(1.0, 1.0);
	if (pNode->Attribute(XML_ATTR_MIN_X))
		m_minCoord.setX(atof(pNode->Attribute(XML_ATTR_MIN_X)));
	if (pNode->Attribute(XML_ATTR_MAX_X))
		m_maxCoord.setX(atof(pNode->Attribute(XML_ATTR_MAX_X)));
	if (pNode->Attribute(XML_ATTR_MIN_Y))
		m_minCoord.setY(atof(pNode->Attribute(XML_ATTR_MIN_Y)));
	if (pNode->Attribute(XML_ATTR_MAX_Y))
		m_maxCoord.setY(atof(pNode->Attribute(XML_ATTR_MAX_Y)));

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

	drawChildren();
}

Meter2D::Meter2D(tinyxml2::XMLElement* pNode): GraphicObject2D(pNode)
{
}

Meter2D::Meter2D(string name, Vector2D origin, Vector2D size, double depth) 
	: GraphicObject2D(name, origin, size, 0.0, depth) //no rotation allowed
{
	//text on the progress bar, shifted a bit to the top-right toward the viewer
	m_pText = new Text2D(name + "/text", Vector2D(0.025, 0.25), 0.1);
	addChild(m_pText);
}

Meter2D::~Meter2D()
{
}


void Meter2D::draw()
{
	double normValue = m_valueRange.normPosInRange(m_value);

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glDisable(GL_BLEND);
	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_LINE_STRIP);
		glVertex2d(0.0, 0.0);
		glVertex2d(0.0, 1.0);
		glVertex2d(1.0, 1.0);
		glVertex2d(1.0, 0.0);
		glVertex2d(0.0, 0.0);
	glEnd();

	glBegin(GL_QUADS);
		glColor3fv(m_startColor.rgba());
		glVertex2d(0.0, 0.0);
		glColor3fv(m_valueColor.rgba());
		glVertex2d(normValue, 0.0);
		glVertex2d(normValue, 1.0);
		glColor3fv(m_startColor.rgba());
		glVertex2d(0.0, 1.0);
	glEnd();

	drawChildren();
}

void Meter2D::setValue(double value)
{
	m_value = m_valueRange.clamp(value);

	double normValue = m_valueRange.normPosInRange(m_value);

	//udpate the color
	m_valueColor = m_startColor.lerp(m_endColor, normValue);

	//update the text
	char buffer[1024];
	sprintf_s(buffer, 1024, "%s: %.4f", m_name.c_str(), value);
	m_pText->set(string(buffer));
}

BoundingBox2D& GraphicObject2D::boundingBox()
{
	return m_bb;
}

FunctionViewer::FunctionViewer(string name, Vector2D origin, Vector2D size, double depth, Material* pMaterial)
	:Sprite2D(name, origin, size, depth, pMaterial)
{}

FunctionViewer3D::FunctionViewer3D(string name, Vector2D origin, Vector2D size, unsigned int pixelRes, double depth)
	:FunctionViewer(name, origin, size, depth, new UnlitLiveTextureMaterial(pixelRes, pixelRes))
{
}


FunctionViewer3D::~FunctionViewer3D()
{
}

void FunctionViewer3D::update(const vector<double>& pBuffer)
{
	((UnlitLiveTextureMaterial*)m_pMaterial)->updateTexture(pBuffer);
}

FunctionViewer2D::FunctionViewer2D(string name, Vector2D origin, Vector2D size, unsigned int pixelRes, double depth = 0)
	:FunctionViewer(name, origin, size, depth) //no material
{
	m_lastValues = vector<double>(pixelRes);
}

FunctionViewer2D::~FunctionViewer2D()
{}

void FunctionViewer2D::update(const vector<double>& pBuffer)
{
	double minValue = pBuffer[0];
	double maxValue = pBuffer[0];
	for (size_t i = 1; i < pBuffer.size(); ++i)
	{
		minValue = std::min(minValue, pBuffer[i]);
		maxValue = std::max(maxValue, pBuffer[i]);
	}
	double invValueRange;
	if (maxValue - minValue > 0)
		invValueRange = 1.0 / (maxValue - minValue);
	else invValueRange = 1.0;
	for (size_t i = 0; i < pBuffer.size(); ++i)
	{
		m_lastValues[i] = minValue + (pBuffer[i] - minValue) * invValueRange;
	}
}

void draw()
{
	glBegin(GL_LINES);

	glEnd();
}
