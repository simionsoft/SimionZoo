#pragma once
namespace tinyxml2 { class XMLElement; }

//loading functions
#include "bindings.h"
class Color;
class Matrix44;
class Transform3D;
class Transform2D;
class BoundingBox3D;
class BoundingBox2D;
class BoundingCylinder;

#include "renderer.h"
#include "xml-tags.h"

namespace XML
{
	template <typename T>
	double loadBindableValue(tinyxml2::XMLElement* pNode, const char* xmlTag, T& obj)
	{
		double value= 0.0;
		const char* pBindingName;
		tinyxml2::XMLElement* pChild = pNode->FirstChildElement(xmlTag);
		if (pChild)
		{
			if (pChild->GetText())
				value = atof(pChild->GetText());
			pBindingName = pChild->Attribute(XML_ATTR_BINDING);
			if (pBindingName)
			{
				double offset = 0.0;
				double multiplier = 1.0;
				if (pChild->Attribute(XML_ATTR_OFFSET))
					offset = atof(pChild->Attribute(XML_ATTR_OFFSET));
				if (pChild->Attribute(XML_ATTR_MULTIPLIER))
					multiplier = atof(pChild->Attribute(XML_ATTR_MULTIPLIER));
				Renderer::get()->registerBinding<T>(pBindingName, obj, xmlTag, offset, multiplier);
			}
			return value;
		}
		return value;
	}
	void loadColladaMatrix(const char* pText, Matrix44& outMatrix);
	void loadVector3D(const char* pText, Vector3D& outVec);

	void load(tinyxml2::XMLElement* pNode, Vector3D& outVec);
	void load(tinyxml2::XMLElement* pNode, Point3D& outVec);
	void load(tinyxml2::XMLElement* pNode, Quaternion& outVec);
	void load(tinyxml2::XMLElement* pNode, Vector2D& outVec);
	void load(tinyxml2::XMLElement* pNode, Point2D& outVec);
	void load(tinyxml2::XMLElement* pNode, Transform3D& transform);
	void load(tinyxml2::XMLElement* pNode, Transform2D& transform);
	void load(tinyxml2::XMLElement* pNode, Color& color);
	void load(tinyxml2::XMLElement* pNode, BoundingBox3D& box);
	void load(tinyxml2::XMLElement* pNode, BoundingBox2D& box);
	void load(tinyxml2::XMLElement* pNode, BoundingCylinder& cylinder);
}
#include <vector>


template <typename T>
void loadChildren(tinyxml2::XMLElement* pParentNode, const char* xmlTag, std::vector<T*> &outVec)
{
	T* pObj;
	int x = 0;
	tinyxml2::XMLElement *pNode = pParentNode->FirstChildElement(xmlTag);
	while (pNode)
	{
		pObj = T::getInstance(pNode);
		if (pObj)
			outVec.push_back(pObj);
		else
			++x;
		pNode = pNode->NextSiblingElement(xmlTag);
	}
}

unsigned int countChildren(tinyxml2::XMLElement* pParentNode, const char* xmlTag);