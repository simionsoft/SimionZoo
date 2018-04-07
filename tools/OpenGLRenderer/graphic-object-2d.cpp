#include "stdafx.h"
#include "graphic-object-2d.h"
#include "basic-shapes-2d.h"
#include "material.h"
#include "renderer.h"
#include "text.h"
#include "xml-load.h"

GraphicObject2D::GraphicObject2D(string name)
{
	m_name = name;
}

GraphicObject2D::GraphicObject2D(string name, Vector2D origin, Vector2D size, int depth)
	: SceneActor2D()
{
	m_name = name;

	m_transform.setTranslation(Vector2D(origin.x(), origin.y()));
	m_transform.setScale(Vector2D(size.x(), size.y()));
	m_transform.setDepth(depth);
}

GraphicObject2D::GraphicObject2D(tinyxml2::XMLElement* pNode) : SceneActor2D(pNode)
{
	if (pNode->Attribute(XML_TAG_NAME_ATTR))
		m_name = string(pNode->Attribute(XML_TAG_NAME_ATTR));
}

GraphicObject2D* GraphicObject2D::getInstance(tinyxml2::XMLElement* pNode)
{
	const char* name = pNode->Name();
	if (!strcmp(pNode->Name(), XML_TAG_SPRITE_2D))
		return new Sprite2D(pNode);

	return nullptr;
}

GraphicObject2D::~GraphicObject2D()
{
}



//void GraphicObject2D::setTransform()
//{
//	glMatrixMode(GL_MODELVIEW);
//	glPushMatrix();
//	double* pMatrix = m_transform.getOpenGLMatrix();
//	glMultMatrixd(pMatrix);
//}
//
//void GraphicObject2D::restoreTransform()
//{
//	glPopMatrix();
//}



