#include "stdafx.h"
#include "xml-load-utils.h"
#include "vector3d.h"
#include "vector2d.h"
#include "transform3d.h"
#include "color.h"
#include "xml-load-utils.h"
#include "renderer.h"
#include "bounding-box.h"
#include "bounding-cylinder.h"

void Quaternion::update(std::string key, double value)
{
	if (key == XML_TAG_X) setX(value);
	else if (key == XML_TAG_Y) setY(value);
	else if (key == XML_TAG_Z) setZ(value);
	else if (key == XML_TAG_W) setW(value);
	else if (key == XML_TAG_YAW) setYaw(value);
	else if (key == XML_TAG_PITCH) setPitch(value);
	else if (key == XML_TAG_ROLL) setRoll(value);
}


void Vector3D::update(std::string key, double value)
{
	if (key == XML_TAG_X) setX(value);
	else if (key == XML_TAG_Y) setY(value);
	else if (key == XML_TAG_Z) setZ(value);
}

void Vector2D::update(std::string key, double value)
{
	if (key == XML_TAG_X) setX(value);
	else if (key == XML_TAG_Y) setY(value);
}

bool loadBindableValue(tinyxml2::XMLElement* pNode, const char* xmlTag, double& value, Bindable* pObj)
{
	const char* pBindingName;
	tinyxml2::XMLElement* pChild = pNode->FirstChildElement(xmlTag);
	if (pChild)
	{
		if (pChild->GetText())
			value= atof(pChild->GetText());
		pBindingName = pChild->Attribute(XML_ATTR_BINDING);
		if (pBindingName)
			CRenderer::get()->registerBinding(pBindingName,pObj,xmlTag);
		return true;
	}
	return false;
}

void Vector3D::load(tinyxml2::XMLElement* pNode)
{
	loadBindableValue(pNode, XML_TAG_X, m_x, this);
	loadBindableValue(pNode, XML_TAG_Y, m_y, this);
	loadBindableValue(pNode, XML_TAG_Z, m_z, this);
}

void Quaternion::load(tinyxml2::XMLElement* pNode)
{
	loadBindableValue(pNode, XML_TAG_X, m_x, this);
	loadBindableValue(pNode, XML_TAG_Y, m_y, this);
	loadBindableValue(pNode, XML_TAG_Z, m_z, this);
	loadBindableValue(pNode, XML_TAG_W, m_w, this);

	if (loadBindableValue(pNode, XML_TAG_YAW, m_yaw, this)
		|| loadBindableValue(pNode, XML_TAG_PITCH, m_pitch, this)
		|| loadBindableValue(pNode, XML_TAG_ROLL, m_roll, this))
		m_bOrientationsSet= true;
}


void Transform3D::load(tinyxml2::XMLElement* pNode)
{
	tinyxml2::XMLElement *pChild;

	pChild = pNode->FirstChildElement(XML_TAG_TRANSLATION);
	if (pChild)
		m_translation.load(pChild);

	pChild = pNode->FirstChildElement(XML_TAG_ROTATION);
	if (pChild)
		m_rotation.load(pChild);

	pChild = pNode->FirstChildElement(XML_TAG_SCALE);
	if (pChild)
		m_scale.load(pChild);
}

void Color::load(tinyxml2::XMLElement* pNode)
{
	if (pNode)
	{
		if (pNode->FirstChildElement(XML_TAG_R))
			_values[0]= (float)atof(pNode->FirstChildElement(XML_TAG_R)->GetText());
		if (pNode->FirstChildElement(XML_TAG_G))
			_values[1] = (float)atof(pNode->FirstChildElement(XML_TAG_G)->GetText());
		if (pNode->FirstChildElement(XML_TAG_B))
			_values[2] = (float)atof(pNode->FirstChildElement(XML_TAG_B)->GetText());
		if (pNode->FirstChildElement(XML_TAG_A))
			_values[3] = (float)atof(pNode->FirstChildElement(XML_TAG_A)->GetText());
	}
}

void BoundingBox3D::load(tinyxml2::XMLElement* pNode)
{
	if (pNode)
	{
		m_bSet = true;
		if (pNode->FirstChildElement(XML_TAG_MIN))
			m_min.load(pNode->FirstChildElement(XML_TAG_MIN));
		if (pNode->FirstChildElement(XML_TAG_MAX))
			m_max.load(pNode->FirstChildElement(XML_TAG_MAX));
	}
}

void BoundingCylinder::load(tinyxml2::XMLElement* pNode)
{
	if (pNode)
	{
		m_bSet = true;
		if (pNode->FirstChildElement(XML_TAG_RADIUS))
			m_radius = atoi(pNode->FirstChildElement(XML_TAG_RADIUS)->GetText());
	}
}

void loadColladaMatrix(const char* pText, Matrix44& outMatrix)
{
	//Collada uses column-major matrices:
	//https://forums.khronos.org/showthread.php/11010-matrix-and-collada
	const char* p = pText;
	size_t charCount = strlen(p);
	int col= 0, row= 0;
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