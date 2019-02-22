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
	if (!strcmp(name, XML_TAG_SPRITE_2D))
		return new Sprite2D(pNode);
	if (!strcmp(name, XML_TAG_TEXT))
		return new Text2D(pNode);

	return nullptr;
}

GraphicObject2D::~GraphicObject2D()
{
	//The renderer doesn't know about children, so we have to delete them here
	for (GraphicObject2D* pChild : m_children)
		delete pChild;
}
