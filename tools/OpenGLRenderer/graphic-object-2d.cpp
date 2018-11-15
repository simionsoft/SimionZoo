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

GraphicObject2D::GraphicObject2D(string name, Vector2D origin, Vector2D size, double rotation, double depth)
	: SceneActor2D(origin, size, rotation, depth)
{
	m_name = name;
}

GraphicObject2D::GraphicObject2D(tinyxml2::XMLElement* pNode) : SceneActor2D(pNode)
{
	if (pNode->Attribute(XML_TAG_NAME_ATTR))
		m_name = string(pNode->Attribute(XML_TAG_NAME_ATTR));

	loadChildren<GraphicObject2D>(pNode, nullptr, m_children);
}

void GraphicObject2D::addChild(GraphicObject2D* child)
{
	m_children.push_back(child);
}

vector<GraphicObject2D*>& GraphicObject2D::getChildren()
{
	return m_children;
}

void GraphicObject2D::drawChildren()
{
	for (GraphicObject2D* pChild : m_children)
	{
		pChild->setTransform();
		pChild->draw();
		pChild->restoreTransform();
	}
}

GraphicObject2D* GraphicObject2D::getInstance(tinyxml2::XMLElement* pNode)
{
	const char* name = pNode->Name();
	if (!strcmp(pNode->Name(), XML_TAG_SPRITE_2D))
		return new Sprite2D(pNode);
	if (!strcmp(pNode->Name(), XML_TAG_TEXT))
		return new Text2D(pNode);

	return nullptr;
}

GraphicObject2D::~GraphicObject2D()
{
	//The renderer doesn't know about children, so we have to delete them here
	for (GraphicObject2D* pChild : m_children)
		delete pChild;
}
