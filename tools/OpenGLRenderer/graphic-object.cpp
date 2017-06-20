#include "stdafx.h"
#include "graphic-object.h"
#include "material.h"
#include "xml-load-utils.h"
#include "basic-shapes.h"
#include "collada-model.h"
#include "mesh.h"
#include "bounding-cylinder.h"
#include <algorithm>



CGraphicObject::CGraphicObject(string name)
{
	m_name = name;
}

CGraphicObject::CGraphicObject(tinyxml2::XMLElement* pNode): CSceneActor(pNode)
{
	if (pNode->Attribute(XML_TAG_NAME_ATTR))
		m_name= string(pNode->Attribute(XML_TAG_NAME_ATTR));
}

CGraphicObject* CGraphicObject::getInstance(tinyxml2::XMLElement* pNode)
{
	const char* name = pNode->Name();
	if (!strcmp(pNode->Name(), XML_TAG_COLLADA_MODEL))
		return new CColladaModel(pNode);
	if (!strcmp(pNode->Name(), XML_TAG_BOX))
		return new CBox(pNode);
	if (!strcmp(pNode->Name(), XML_TAG_SPHERE))
		return new CSphere(pNode);
	if (!strcmp(pNode->Name(), XML_TAG_CILINDER))
		return new CCilinder(pNode);
	if (!strcmp(pNode->Name(), XML_TAG_POLYLINE))
		return new CPolyline(pNode);

	return nullptr;
}


CGraphicObject::~CGraphicObject()
{
}

BoundingBox3D CGraphicObject::boundingBox() const
{
	return m_bb;
}


void CGraphicObject::draw()
{
	setTransform();

	for (auto it = m_meshes.begin(); it != m_meshes.end(); ++it)
		(*it)->draw();

	restoreTransform();
}

void CGraphicObject::drawBoundingBox()
{
	setTransform();
	m_bb.draw();
	restoreTransform();
}

void CGraphicObject::updateBoundingBox()
{
	m_bb.reset();
	for (auto it = m_meshes.begin(); it != m_meshes.end(); ++it)
		(*it)->updateBoundingBox(m_bb);
}

void CGraphicObject::fitToBoundingBox(BoundingBox3D& newBB)
{
	Vector3D newBBSize,bbSize;
	Vector3D newBBCenter, bbCenter;
	Vector3D translation, scale;

	if (!newBB.bSet())
		return;
	
	newBBCenter = newBB.center();
	bbCenter = m_bb.center();
	newBBSize = newBB.size();
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

void CGraphicObject::fitToBoundingCylinder(BoundingCylinder& newBC)
{
	BoundingCylinder currentBC;
	/*Point3D centroid;*/
	double sampleRadius= std::numeric_limits<double>::min();
	
	//We assume the centroid in the Y=0 plane is (0,0)

	////find the centroid
	//int numSamples = 0;
	//for (auto it = m_meshes.begin(); it != m_meshes.end(); ++it)
	//{
	//	for (int i = 0; i < (*it)->getNumPositions(); ++it)
	//	{
	//		centroid += m_transform*(*it)->getPosition(i);
	//		++numSamples;
	//	}
	//}
	//centroid /= std::max(1,numSamples);

	//calculate the BC
	for (auto it = m_meshes.begin(); it != m_meshes.end(); ++it)
	{
		for (int i = 0; i < (*it)->getNumPositions(); ++i)
		{
			sampleRadius = std::max(sampleRadius,
				sqrt((*it)->getPosition(i).x()*(*it)->getPosition(i).x()
					+ (*it)->getPosition(i).z()*(*it)->getPosition(i).z()));
		}
	}
	double scaleFactor = newBC.radius() / sampleRadius;
	//transform vertices
	for (auto it = m_meshes.begin(); it != m_meshes.end(); ++it)
	{
		(*it)->transformVertices(Vector3D(0, 0, 0), Vector3D(scaleFactor, scaleFactor, scaleFactor));
	}
	updateBoundingBox();
}