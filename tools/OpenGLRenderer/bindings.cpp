#include "stdafx.h"
#include "bindings.h"
#include "../GeometryLib/vector3d.h"
#include "../GeometryLib/vector2d.h"
#include "../GeometryLib/transform3d.h"
#include "../GeometryLib/bounding-box.h"
#include "../GeometryLib/bounding-cylinder.h"
#include "color.h"
#include "renderer.h"

bool loadBindableValue(tinyxml2::XMLElement* pNode, const char* xmlTag, double& value, Bindable* pObj)
{
	const char* pBindingName;
	tinyxml2::XMLElement* pChild = pNode->FirstChildElement(xmlTag);
	if (pChild)
	{
		if (pChild->GetText())
			value = atof(pChild->GetText());
		pBindingName = pChild->Attribute(XML_ATTR_BINDING);
		if (pBindingName)
			CRenderer::get()->registerBinding(pBindingName, pObj, xmlTag);
		return true;
	}
	return false;
}

void BindableQuaternion::update(std::string key, double value)
{
	if (key == XML_TAG_X) setX(value);
	else if (key == XML_TAG_Y) setY(value);
	else if (key == XML_TAG_Z) setZ(value);
	else if (key == XML_TAG_W) setW(value);
	else if (key == XML_TAG_YAW) setYaw(value);
	else if (key == XML_TAG_PITCH) setPitch(value);
	else if (key == XML_TAG_ROLL) setRoll(value);
}


void BindableVector3D::update(std::string key, double value)
{
	if (key == XML_TAG_X) setX(value);
	else if (key == XML_TAG_Y) setY(value);
	else if (key == XML_TAG_Z) setZ(value);
}

void BindableVector2D::update(std::string key, double value)
{
	if (key == XML_TAG_X) setX(value);
	else if (key == XML_TAG_Y) setY(value);
}

void BindableVector3D::load(tinyxml2::XMLElement* pNode)
{
	loadBindableValue(pNode, XML_TAG_X, m_x, this);
	loadBindableValue(pNode, XML_TAG_Y, m_y, this);
	loadBindableValue(pNode, XML_TAG_Z, m_z, this);
}
void BindablePoint3D::load(tinyxml2::XMLElement* pNode)
{
	loadBindableValue(pNode, XML_TAG_X, m_x, this);
	loadBindableValue(pNode, XML_TAG_Y, m_y, this);
	loadBindableValue(pNode, XML_TAG_Z, m_z, this);
}

void BindableQuaternion::load(tinyxml2::XMLElement* pNode)
{
	loadBindableValue(pNode, XML_TAG_X, m_x, this);
	loadBindableValue(pNode, XML_TAG_Y, m_y, this);
	loadBindableValue(pNode, XML_TAG_Z, m_z, this);
	loadBindableValue(pNode, XML_TAG_W, m_w, this);

	if (loadBindableValue(pNode, XML_TAG_YAW, m_yaw, this))
		m_bOrientationsSet = true;
	if (loadBindableValue(pNode, XML_TAG_PITCH, m_pitch, this))
		m_bOrientationsSet = true;
	if (loadBindableValue(pNode, XML_TAG_ROLL, m_roll, this))
		m_bOrientationsSet = true;
}


void LoadableTransform3D::load(tinyxml2::XMLElement* pNode)
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

void LoadableColor::load(tinyxml2::XMLElement* pNode)
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

void LoadableBoundingBox3D::load(tinyxml2::XMLElement* pNode)
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

void LoadableBoundingBox2D::load(tinyxml2::XMLElement* pNode)
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

void LoadableBoundingCylinder::load(tinyxml2::XMLElement* pNode)
{
	if (pNode)
	{
		m_bSet = true;
		if (pNode->FirstChildElement(XML_TAG_RADIUS))
			m_radius = atoi(pNode->FirstChildElement(XML_TAG_RADIUS)->GetText());
	}
}

