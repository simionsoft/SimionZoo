#include "stdafx.h"
#include "xml-load.h"
#include "../GeometryLib/matrix44.h"
#include "../GeometryLib/transform3d.h"
#include "../GeometryLib/transform2d.h"
#include "../GeometryLib/bounding-box.h"
#include "../GeometryLib/bounding-cylinder.h"
#include "bindings.h"
#include "renderer.h"
#include "color.h"

//////////////////////////////////////////////////////////////////////
//UTILITY FUNCTIONS

unsigned int countChildren(tinyxml2::XMLElement* pParentNode, const char* xmlTag)
{
	unsigned int count = 0;
	tinyxml2::XMLElement* pChild = pParentNode->FirstChildElement(xmlTag);
	while (pChild)
	{
		++count;
		pChild = pChild->NextSiblingElement();
	}
	return count;
}

bool childExists(tinyxml2::XMLElement* pNode, const char* xmlTag)
{
	tinyxml2::XMLElement* pChild = pNode->FirstChildElement(xmlTag);
	if (pChild)
		return true;
	else
		return false;
}

namespace XML
{

	void loadColladaMatrix(const char* pText, Matrix44& outMatrix)
	{
		//Collada uses column-major matrices:
		//https://forums.khronos.org/showthread.php/11010-matrix-and-collada
		const char* p = pText;
		size_t charCount = strlen(p);
		int col = 0, row = 0;
		for (int row = 0; row < 4; ++row)
		{
			for (int col = 0; col < 4; ++col)
			{
				outMatrix.set(col, row, atof(p));

				while ((size_t)(p - pText) < charCount && *p != ' ') ++p;
				while (*p == ' ') ++p;
			}
		}
	}


	void loadVector3D(const char* pText, Vector3D& outVec)
	{
		const char* p = pText;
		size_t charCount = strlen(p);

		outVec.setX(atof(p));
		while ((size_t)(p - pText) < charCount && *p != ' ') ++p;
		while (*p == ' ') ++p;
		outVec.setY(atof(p));
		while ((size_t)(p - pText) < charCount && *p != ' ') ++p;
		while (*p == ' ') ++p;
		outVec.setZ(atof(p));
		while ((size_t)(p - pText) < charCount && *p != ' ') ++p;
	}

	void load(tinyxml2::XMLElement* pNode, Vector3D& vector)
	{
		if (childExists(pNode, XML_TAG_X))
			vector.setX(loadBindableValue(pNode, XML_TAG_X, vector));
		if (childExists(pNode, XML_TAG_Y))
			vector.setY(loadBindableValue(pNode, XML_TAG_Y, vector));
		if (childExists(pNode, XML_TAG_Z))
			vector.setZ(loadBindableValue(pNode, XML_TAG_Z, vector));
	}
	void load(tinyxml2::XMLElement* pNode, Point3D& point)
	{
		if (childExists(pNode, XML_TAG_X))
			point.setX(loadBindableValue(pNode, XML_TAG_X, point));
		if (childExists(pNode, XML_TAG_Y))
			point.setY(loadBindableValue(pNode, XML_TAG_Y, point));
		if (childExists(pNode, XML_TAG_Z))
			point.setZ(loadBindableValue(pNode, XML_TAG_Z, point));
	}

	void load(tinyxml2::XMLElement* pNode, Vector2D& vector)
	{
		if (childExists(pNode, XML_TAG_X))
			vector.setX(loadBindableValue(pNode, XML_TAG_X, vector));
		if (childExists(pNode, XML_TAG_Y))
			vector.setY(loadBindableValue(pNode, XML_TAG_Y, vector));
	}
	void load(tinyxml2::XMLElement* pNode, Point2D& point)
	{
		if (childExists(pNode, XML_TAG_X))
			point.setX(loadBindableValue(pNode, XML_TAG_X, point));
		if (childExists(pNode, XML_TAG_Y))
			point.setY(loadBindableValue(pNode, XML_TAG_Y, point));
	}


	void load(tinyxml2::XMLElement* pNode, Quaternion& quat)
	{
		if (childExists(pNode, XML_TAG_X))
			quat.setX(loadBindableValue(pNode, XML_TAG_X, quat));
		if (childExists(pNode, XML_TAG_Y))
			quat.setY(loadBindableValue(pNode, XML_TAG_Y, quat));
		if (childExists(pNode, XML_TAG_Z))
			quat.setZ(loadBindableValue(pNode, XML_TAG_Z, quat));
		if (childExists(pNode, XML_TAG_W))
			quat.setW(loadBindableValue(pNode, XML_TAG_W, quat));

		if (childExists(pNode, XML_TAG_YAW))
			quat.setYaw(loadBindableValue(pNode, XML_TAG_YAW, quat));
		if (childExists(pNode, XML_TAG_PITCH))
			quat.setPitch(loadBindableValue(pNode, XML_TAG_PITCH, quat));
		if (childExists(pNode, XML_TAG_ROLL))
			quat.setRoll(loadBindableValue(pNode, XML_TAG_ROLL, quat));
	}


	void load(tinyxml2::XMLElement* pNode, Transform3D& transform)
	{
		tinyxml2::XMLElement *pChild;

		pChild = pNode->FirstChildElement(XML_TAG_TRANSLATION);
		if (pChild)
			load(pChild, transform.translation());

		pChild = pNode->FirstChildElement(XML_TAG_ROTATION);
		if (pChild)
			load(pChild, transform.rotation());

		pChild = pNode->FirstChildElement(XML_TAG_SCALE);
		if (pChild)
			load(pChild, transform.scale());
	}

	void load(tinyxml2::XMLElement* pNode, Transform2D& transform)
	{
		tinyxml2::XMLElement *pChild;

		pChild = pNode->FirstChildElement(XML_TAG_TRANSLATION);
		if (pChild)
			load(pChild, transform.translation());

		pChild = pNode->FirstChildElement(XML_TAG_ROTATION);
		if (pChild)
			transform.setRotation(atof(pNode->FirstChildElement(XML_TAG_ROTATION)->GetText()));

		pChild = pNode->FirstChildElement(XML_TAG_SCALE);
		if (pChild)
			load(pChild, transform.scale());

		pChild = pNode->FirstChildElement(XML_TAG_DEPTH);
		if (pChild)
			transform.setDepth(atof(pNode->FirstChildElement(XML_TAG_DEPTH)->GetText()));
	}


	void load(tinyxml2::XMLElement* pNode, Color& color)
	{
		if (pNode)
		{
			if (pNode->FirstChildElement(XML_TAG_R))
				color.setR((float)atof(pNode->FirstChildElement(XML_TAG_R)->GetText()));
			if (pNode->FirstChildElement(XML_TAG_G))
				color.setG((float)atof(pNode->FirstChildElement(XML_TAG_G)->GetText()));
			if (pNode->FirstChildElement(XML_TAG_B))
				color.setB((float)atof(pNode->FirstChildElement(XML_TAG_B)->GetText()));
			if (pNode->FirstChildElement(XML_TAG_A))
				color.setA((float)atof(pNode->FirstChildElement(XML_TAG_A)->GetText()));
		}
	}

	void load(tinyxml2::XMLElement* pNode, BoundingBox3D& box)
	{
		if (pNode)
		{
			if (childExists(pNode,XML_TAG_MIN))
				load(pNode->FirstChildElement(XML_TAG_MIN), box.min());
			if (pNode->FirstChildElement(XML_TAG_MAX))
				load(pNode->FirstChildElement(XML_TAG_MAX), box.max());
		}
	}

	void load(tinyxml2::XMLElement* pNode, BoundingBox2D& box)
	{
		if (pNode)
		{
			if (pNode->FirstChildElement(XML_TAG_MIN))
				load(pNode->FirstChildElement(XML_TAG_MIN), box.min());
			if (pNode->FirstChildElement(XML_TAG_MAX))
				load(pNode->FirstChildElement(XML_TAG_MAX), box.max());
		}
	}

	void load(tinyxml2::XMLElement* pNode, BoundingCylinder& cylinder)
	{
		if (pNode)
		{
			if (pNode->FirstChildElement(XML_TAG_RADIUS))
				cylinder.setRadius(atoi(pNode->FirstChildElement(XML_TAG_RADIUS)->GetText()));
		}
	}
}