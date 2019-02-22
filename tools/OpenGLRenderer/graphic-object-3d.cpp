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
#include "graphic-object-3d.h"
#include "material.h"
#include "xml-load.h"
#include "basic-shapes-3d.h"
#include "collada-model.h"
#include "mesh.h"
#include "../GeometryLib/bounding-cylinder.h"
#include <algorithm>
#include <math.h>


GraphicObject3D::GraphicObject3D(string name)
{
	m_name = name;
}

GraphicObject3D::GraphicObject3D(tinyxml2::XMLElement* pNode): SceneActor3D(pNode)
{
	if (pNode->Attribute(XML_TAG_NAME_ATTR))
		m_name= string(pNode->Attribute(XML_TAG_NAME_ATTR));

	loadChildren<GraphicObject3D>(pNode, nullptr, m_children);
}

GraphicObject3D* GraphicObject3D::getInstance(tinyxml2::XMLElement* pNode)
{
	const char* name = pNode->Name();
	if (!strcmp(name, XML_TAG_COLLADA_MODEL))
		return new ColladaModel(pNode);
	if (!strcmp(name, XML_TAG_BOX))
		return new Box(pNode);
	if (!strcmp(name, XML_TAG_SPHERE))
		return new Sphere(pNode);
	if (!strcmp(name, XML_TAG_CILINDER))
		return new Cilinder(pNode);
	if (!strcmp(name, XML_TAG_POLYLINE))
		return new PolyLine(pNode);

	return nullptr;
}

void GraphicObject3D::addChild(GraphicObject3D* child)
{
	m_children.push_back(child);
}

vector<GraphicObject3D*>& GraphicObject3D::getChildren()
{
	return m_children;
}

void GraphicObject3D::drawChildren()
{
	for (GraphicObject3D* pChild : m_children)
	{
		pChild->setTransform();
		pChild->draw();
		pChild->restoreTransform();
	}
}

GraphicObject3D::~GraphicObject3D()
{
	//The renderer doesn't know about children, so we have to delete them here
	for (GraphicObject3D* pChild : m_children)
		delete pChild;
}

BoundingBox3D GraphicObject3D::boundingBox()
{
	return m_transform.transformMatrix()*m_bb;
}


void GraphicObject3D::draw()
{
	setTransform();

	for (auto it = m_meshes.begin(); it != m_meshes.end(); ++it)
		(*it)->draw();

	drawChildren();

	restoreTransform();
}


void GraphicObject3D::updateBoundingBox()
{
	m_bb.reset();
	for (auto it = m_meshes.begin(); it != m_meshes.end(); ++it)
		(*it)->updateBoundingBox(m_bb);
}

void GraphicObject3D::fitToBoundingBox(BoundingBox3D* newBB)
{
	Vector3D newBBSize,bbSize;
	Vector3D newBBCenter, bbCenter;
	Vector3D translation, scale;

	if (!newBB->bSet())
		return;
	
	newBBCenter = newBB->center();
	bbCenter = m_bb.center();
	newBBSize = newBB->size();
	bbSize = m_bb.size();

	translation= Vector3D(newBBCenter.x() - bbCenter.x()
		, newBBCenter.y() - bbCenter.y()
		, newBBCenter.z() - bbCenter.z());

	scale = Vector3D(newBBSize.x() / bbSize.x(), newBBSize.y() / bbSize.y(), newBBSize.z() / bbSize.z());
	double minRatio = std::min(scale.x(), std::min(scale.y(), scale.z()));
	scale.setX(std::min(scale.x(), minRatio));
	scale.setY(std::min(scale.y(), minRatio));
	scale.setZ(std::min(scale.z(), minRatio));

	for (auto it = m_meshes.begin(); it != m_meshes.end(); ++it)
		(*it)->transformVertices(translation, scale);

	updateBoundingBox();
}

void GraphicObject3D::fitToBoundingCylinder(BoundingCylinder* newBC)
{
	BoundingCylinder currentBC;
	/*Point3D centroid;*/
	double sampleRadius= std::numeric_limits<double>::min();
	
	//We assume the centroid in the Y=0 plane is (0,0)

	//calculate the BC
	for (auto it = m_meshes.begin(); it != m_meshes.end(); ++it)
	{
		for (unsigned int i = 0; i < (*it)->getNumPositions(); ++i)
		{
			sampleRadius = std::max(sampleRadius,
				sqrt((*it)->getPosition(i).x()*(*it)->getPosition(i).x()
					+ (*it)->getPosition(i).z()*(*it)->getPosition(i).z()));
		}
	}
	double scaleFactor = newBC->radius() / sampleRadius;
	//transform vertices
	Vector3D min(0, 0, 0);
	Vector3D scale(scaleFactor, scaleFactor, scaleFactor);
	for (auto it = m_meshes.begin(); it != m_meshes.end(); ++it)
	{
		(*it)->transformVertices(min, scale);
	}
	updateBoundingBox();
}